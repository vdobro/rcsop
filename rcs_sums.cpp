#include "rcs_sums.h"

typedef Eigen::Hyperplane<double, 3> plane;

static Vector3d get_right(const Image& image) {
    Vector3d local_right;
    local_right.setZero();
    local_right.x() = 1;
    auto direction = transform_to_world(image, local_right) - get_image_position(image);
    return direction.normalized();
}

static Vector3d get_up(const Image& image) {
    Vector3d local_right;
    local_right.setZero();
    local_right.y() = -1;
    auto direction = transform_to_world(image, local_right) - get_image_position(image);
    return direction.normalized();
}

struct relative_point {
    Vector3d position;
    point_id_t id = 0;
    double distance = 0;
    double vertical_angle = 0;
    double horizontal_angle = 0;
};

#define ARC_SIN(X) (asin(X) * 180.f / M_PI)

static vector<relative_point> get_point_angles(const Image& image,
                                               const scored_point_map& points) {

    auto image_pos = get_image_position(image);
    auto right = get_right(image);
    auto up = get_up(image);

    const plane vertical_plane = plane(right, image_pos);
    const plane horizontal_plane = plane(up, image_pos);

    vector<relative_point> result;
    for (const auto& point_pair: points) {
        auto id = point_pair.first;
        auto point = point_pair.second;
        auto point_position = point.position();

        auto line_to_vertical_plane = model_line(point_position, vertical_plane.normal());
        auto line_to_horizontal_plane = model_line(point_position, horizontal_plane.normal());

        auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
        auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

        auto distance_to_camera = (point_position - image_pos).norm();

        auto horizontal_angle = ARC_SIN(distance_to_vertical_plane / distance_to_camera);
        auto vertical_angle = ARC_SIN(distance_to_horizontal_plane / distance_to_camera);

        relative_point point_info = {
                .position = point_position,
                .id = id,
                .distance = distance_to_camera,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
        result.push_back(point_info);
    }
    return result;
}

#define HORIZONTAL_SPREAD (45.0 / 2.0)
#define VERTICAL_SPREAD (15.0 / 2.0)
#define GAUSS_SIGMA_2 (3 * 3)

//TODO numeric integration on the fly
//(numeric) sum of Riemann integral of 1 / (exp (-(3x)^2)) from -3 to +3
#define GAUSS_INTEGRAL_FACTOR 1.69260614115415

//TODO 2d Gauss?
static double calc_gauss(double value) {
    const auto exp_arg = -1 * GAUSS_SIGMA_2 * value * value / HORIZONTAL_SPREAD;
    const auto result = exp(exp_arg) * GAUSS_INTEGRAL_FACTOR / HORIZONTAL_SPREAD;
    return result;
}

static double rcs_gaussian(const relative_point& point) {
    if (point.vertical_angle > VERTICAL_SPREAD || point.horizontal_angle > HORIZONTAL_SPREAD) {
        return 0;
    }

    return calc_gauss(point.horizontal_angle);
}

void sum_pyramids(const model_ptr& model,
                  const vector<double>& rcs,
                  const string& input_path,
                  const string& output_path) {
    auto images = get_images(*model);
    auto image_count = images.size();

    auto scored_points = get_scored_points(*model);
    for (size_t i = 0; i < image_count; i++) {
        auto image = images[i];
        auto rcs_value = rcs[i];
        auto relevant_points = get_point_angles(image, scored_points);

        for (const auto& point: relevant_points) {
            auto& scored_point = scored_points[point.id];
            auto rcs_distributed = rcs_gaussian(point) * rcs_value;
            scored_point.increment_score(rcs_distributed);
        }
    }

    //TODO coloring
    //write_model(model, output_path);

    auto render_path = output_path + path_separator + "render";
    std::filesystem::remove_all(render_path);
    render_images(model, input_path, render_path, scored_points);
}