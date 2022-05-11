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
    Vector3d local_up;
    local_up.setZero();
    local_up.y() = -1;
    auto direction = transform_to_world(image, local_up) - get_image_position(image);
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
                                               double height_offset,
                                               const scored_point_map& points) {
    auto right = get_right(image);
    auto up = get_up(image);
    Vector3d image_pos = get_image_position(image) + (height_offset * up);

    const plane vertical_plane = plane(right, image_pos);
    const plane horizontal_plane = plane(up, image_pos);

    vector<relative_point> result;
    for (const auto& point_pair: points) {
        auto id = point_pair.first;
        auto point = point_pair.second;
        Vector3d point_position = point.position();

        auto line_to_vertical_plane = model_line(point_position, vertical_plane.normal());
        auto line_to_horizontal_plane = model_line(point_position, horizontal_plane.normal());

        auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
        auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

        auto distance_to_camera = (point_position - image_pos).norm();

        auto horizontal_angle = ARC_SIN(distance_to_vertical_plane / distance_to_camera);
        auto vertical_angle = ARC_SIN(distance_to_horizontal_plane / distance_to_camera);

        relative_point point_info = {
                .position = point.position(),
                .id = id,
                .distance = distance_to_camera,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
        result.push_back(point_info);
    }
    return result;
}

typedef struct {
    double sigma;
    double integral_factor;
    double x_scale;
    double y_scale;
} gauss_options;

static double get_gauss_integral_factor(const double& sigma) {
    auto erf_part = erf(1 / (sigma * sqrt(M_PI)));
    auto integral = sigma * M_SQRT2 * sqrt(M_PI) * erf_part * erf_part;
    return 1 / integral;
}

static double raw_gauss(const double& x,
                        const double& y,
                        const double& sigma) {
    return M_SQRT1_2 * exp(-(x * x + y * y) / (2 * sigma * sigma)) / (sigma * sqrt(M_PI));
}

static double calc_gauss(const relative_point& point,
                         const gauss_options& distribution_options) {
    return raw_gauss(point.horizontal_angle / distribution_options.x_scale,
                     point.vertical_angle / distribution_options.y_scale,
                     distribution_options.sigma)
           * distribution_options.integral_factor
           / (distribution_options.x_scale * distribution_options.y_scale);
}

#define HORIZONTAL_SPREAD (HORIZONTAL_ANGLE / 2.0)
#define VERTICAL_SPREAD (VERTICAL_ANGLE / 2.0)

static double rcs_gaussian(const relative_point& point, const gauss_options& distribution_options) {
    if (point.vertical_angle > VERTICAL_SPREAD || point.horizontal_angle > HORIZONTAL_SPREAD) {
        return 0;
    }

    return calc_gauss(point, distribution_options);
}

static void rcs_sums(const model_ptr& model,
                     const rcs_data& rcs_data,
                     const std::function<double(rcs_height_t, size_t, const relative_point&)>& rcs_mapper,
                     const string& input_path,
                     const string& output_path) {

    auto images = get_images(*model);
#ifdef SINGLE_PROJECTION
    auto first_image = DEFAULT_CAMERA;
    auto image_count = DEFAULT_CAMERA + 1;
    auto heights = vector<long> { DEFAULT_HEIGHT };
#else
    auto first_image = 0;
    auto image_count = images.size();
    auto heights = rcs_data.available_heights();
#endif
    auto world_scale = get_world_scale(CAMERA_DISTANCE, *model);
    auto scored_points = get_scored_points(*model);
    std::unordered_set<point_id_t> touched_points;

    const gauss_options distribution_options = {
            .sigma = STANDARD_DEVIATION,
            .integral_factor = get_gauss_integral_factor(STANDARD_DEVIATION),
            .x_scale = HORIZONTAL_SPREAD,
            .y_scale = VERTICAL_SPREAD,
    };
    for (size_t image_index = first_image; image_index < image_count; image_index++) {
        auto image = images[image_index];

        for (auto& height: heights) {
            auto height_offset = static_cast<double>(height - DEFAULT_HEIGHT) * world_scale;
            auto relevant_points = get_point_angles(image, height_offset, scored_points);
            for (const auto& point: relevant_points) {
                double rcs_value = rcs_mapper(height, image_index, point);
                auto& scored_point = scored_points[point.id];
                double gaussian = rcs_gaussian(point, distribution_options);
                if (gaussian > 0 && !std::isnan(rcs_value)) {
                    auto rcs_distributed = gaussian * rcs_value;
                    scored_point.increment_score(rcs_distributed);
                    touched_points.insert(point.id);
                }
            }
        }
    }
    scored_point_map filtered_points;
    for (auto& point_id: touched_points) {
        filtered_points.insert(std::make_pair(point_id, scored_points.at(point_id)));
    }

    auto render_path = output_path + path_separator + "render";
    std::filesystem::remove_all(render_path);
    render_images(model, input_path, render_path, filtered_points);
}

void accumulate_rcs(const model_ptr& model,
                    const rcs_data& rcs_data,
                    const string& input_path,
                    const string& output_path) {
    rcs_sums(model, rcs_data, [rcs_data](rcs_height_t height, size_t image_index, const relative_point& point) {
        return rcs_data.at_height(height)->rcs()[image_index];
    }, input_path, output_path);
}

static const double RANGE_EPSILON = 3;

static double find_azimuth(double real_distance,
                           const vector<double>& azimuth_values,
                           const vector<long>& ranges) {
    for (int i = 0; i < ranges.size(); i++) {
        auto range = static_cast<double>(ranges[i]);
        if (abs(range - real_distance) <= RANGE_EPSILON) {
            auto azimuth = azimuth_values[i];
            return azimuth;
        }
    }
    return NAN;
}

void accumulate_azimuth(const model_ptr& model,
                        const rcs_data& rcs_data,
                        const string& input_path,
                        const string& output_path) {
    auto world_scale = get_world_scale(CAMERA_DISTANCE, *model);
    rcs_sums(model, rcs_data,
             [rcs_data, world_scale](rcs_height_t height, size_t image_index, const relative_point& point) {
                 auto rcs_row = rcs_data.at_height(height);
                 auto ranges = rcs_row->ranges();
                 //TODO: infer range from image_index?
                 auto azimuth_values = rcs_row->azimuth()[5 * (static_cast<long>(image_index) + 1)];
                 auto point_distance = point.distance / world_scale;
                 auto azimuth_data = find_azimuth(point_distance, azimuth_values, ranges);
                 return azimuth_data;
             }, input_path, output_path);
}