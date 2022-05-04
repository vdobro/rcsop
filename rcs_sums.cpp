#include "utils.h"
#include "scored_point.h"
#include "render_points.h"
#include "colors.h"

typedef Eigen::Hyperplane<double, 3> plane;

static Vector3d get_right(const Image& image) {
    Vector3d local_right;
    local_right.setZero();
    local_right.x() = 1;
    return transform_to_world(image, local_right).normalized();
}

static Vector3d get_up(const Image& image) {
    Vector3d local_right;
    local_right.setZero();
    local_right.y() = -1;
    return transform_to_world(image, local_right).normalized();
}

struct relative_point {
    Vector3d position;
    ulong id = 0;
    double distance = 0;
    double vertical_angle = 0;
    double horizontal_angle = 0;
};

static vector<relative_point> extract_point_data(const Image& image,
                                                 const vector<point_pair>& points) {

    auto image_pos = get_image_position(image);
    auto right = get_right(image);
    auto up = get_up(image);

    const plane vertical_plane = plane(image_pos, right);
    const plane horizontal_plane = plane(image_pos, up);

    return map_vec<point_pair, relative_point>(points, [image_pos, vertical_plane, horizontal_plane](
            const point_pair& point_pair) {
        auto point = point_pair.second;
        auto point_position = point.XYZ();

        auto line_to_vertical_plane = model_line(point_position, vertical_plane.normal());
        auto line_to_horizontal_plane = model_line(point_position, horizontal_plane.normal());

        auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
        auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

        auto distance_to_camera = (point_position - image_pos).norm();

        auto horizontal_angle = asin(distance_to_vertical_plane / distance_to_camera) * 180 / M_PI;
        auto vertical_angle = asin(distance_to_horizontal_plane / distance_to_camera) * 180 / M_PI;

        relative_point point_info = {
                .position = point_position,
                .id = point_pair.first,
                .distance = distance_to_camera,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
        return point_info;
    });
}

#define INTEGRAL_SCALER 1.69260614115415

void sum_pyramids(const model_ptr& model,
                  const vector<double>& rcs,
                  const string& input_path,
                  const string& output_path) {
    auto images = get_images(*model);
    auto image_count = images.size();

    vector<scored_point> scored_points;
    scored_points.resize(model->NumPoints3D());

    const auto rcs_colors = color_values(rcs, map_turbo);
    for (size_t i = 0; i < 1; i++) {
        auto image = images[i];
        auto rcs_value = rcs[i];
        auto points = get_points(*model);
        auto relevant_points = extract_point_data(image, points);

        for (const auto& point : relevant_points) {
            if (point.horizontal_angle <= 45 && point.vertical_angle <= 15) {
                scored_point sp = scored_point(point.position, point.id);
                sp.increment_score(1);
                scored_points.push_back(sp);

                model->Point3D(point.id).Color() = rcs_colors[0];
            }
        }
    }

    write_model(model, output_path);

    auto render_path = output_path + path_separator + "render";
    std::filesystem::remove_all(render_path);
    render_images(model, input_path, render_path, scored_points);
}