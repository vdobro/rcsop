#include "rcs_sums.h"

using std::string;

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

        auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
        auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

        auto distance_to_camera = (point_position - image_pos).norm();

        auto is_to_the_right = (point_position - image_pos).dot(right) > 0;
        auto is_up_above = (point_position - image_pos).dot(up) > 0;

        auto horizontal_angle = (is_to_the_right ? 1 : -1 ) * ARC_SIN(distance_to_vertical_plane / distance_to_camera);
        auto vertical_angle = (is_up_above ? 1 : -1 ) * ARC_SIN(distance_to_horizontal_plane / distance_to_camera);

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
    auto erf_part = erf(1 / (sigma * M_SQRT2));
    auto integral = sigma * M_SQRT2 * sqrt(M_PI) * erf_part * erf_part;
    return 1 / integral;
}

static double raw_gauss(const double& x,
                        const double& y,
                        const double& sigma) {
    return M_SQRT1_2 * exp(-1 * (x * x + y * y) / (2 * sigma * sigma)) / (sigma * sqrt(M_PI));
}

static double calc_gauss(const relative_point& point,
                         const gauss_options& distribution_options) {
    return raw_gauss(point.horizontal_angle / distribution_options.x_scale,
                     point.vertical_angle / distribution_options.y_scale,
                     distribution_options.sigma)
           * distribution_options.integral_factor
           / (distribution_options.x_scale * distribution_options.y_scale);
}

static bool is_inside_ellipse(const relative_point& point, const gauss_options& options) {
    auto x = point.horizontal_angle / options.x_scale;
    auto y = point.vertical_angle / options.y_scale;
    return x * x + y * y <= 1;
}

static double rcs_gaussian(const relative_point& point,
                           const gauss_options& options) {
    if (!is_inside_ellipse(point, options)) {
        return 0;
    }

    return calc_gauss(point, options) / point.distance;
}

#define HORIZONTAL_SPREAD (HORIZONTAL_ANGLE / 2.0)
#define VERTICAL_SPREAD (VERTICAL_ANGLE / 2.0)

static void rcs_sums(const model_ptr& model,
                     const rcs_data& rcs_data,
                     const std::function<double(rcs_height_t, size_t, const relative_point&)>& rcs_mapper,
                     const string& input_path,
                     const string& output_path) {

    auto images = get_images(*model);
#ifdef SINGLE_PROJECTION
    size_t first_image = DEFAULT_CAMERA;
    size_t last_image_plus_one = DEFAULT_CAMERA + 1;
    auto heights = vector<long>{DEFAULT_HEIGHT};
#else
    size_t first_image = 0;
    size_t last_image_plus_one = images.size();
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
    auto time_measure = start_time();
    for (size_t image_index = first_image; image_index < last_image_plus_one; image_index++) {
        auto image = images[image_index];

        for (auto& height: heights) {
            auto height_offset = static_cast<double>(height - DEFAULT_HEIGHT) * world_scale;
            auto relevant_points = get_point_angles(image, height_offset, scored_points);

            std::for_each(std::execution::par_unseq, relevant_points.begin(), relevant_points.end(),
                          [&](relative_point& point) {
                auto rcs_value = rcs_mapper(height, image_index, point);
                auto gaussian = rcs_gaussian(point, distribution_options);
                auto rcs_distributed = gaussian * rcs_value;
                scored_points[point.id].increment_score(rcs_distributed);
            });
        }
    }
    time_measure = log_and_start_next(time_measure,
                                      "Scoring of " + std::to_string(scored_points.size()) + " points for "
                                      + std::to_string(last_image_plus_one) + " images");

    vector<scored_point> filtered_points;
    for (const auto& iter: scored_points) {
        auto old_point = iter.second;
        if (old_point.score() <= 0) {
            continue;
        }
        filtered_points.push_back(old_point);
    }
    time_measure = log_and_start_next(time_measure, "Filtered " + std::to_string(filtered_points.size())
                                                    + " from a total of " + std::to_string(scored_points.size())
                                                    + " points");

    auto render_path = output_path + path_separator + "render";
    std::filesystem::remove_all(render_path);
    render_images(model, input_path, render_path, filtered_points);

    log_and_start_next(time_measure, "(Total) rendering done.");
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
                           const double first) {
    auto index = lround((real_distance - first) / (2 * RANGE_EPSILON));
    return azimuth_values[index];
}

using std::make_pair;

static map<rcs_height_t, map<size_t, long>> get_height_map(const rcs_data& rcs_data) {
    map<rcs_height_t, map<size_t, long>> result;
    for (auto height: rcs_data.available_heights()) {
        auto row = rcs_data.at_height(height);
        map<size_t, long> image_to_angle;
        auto angles = row->angles();
        for (size_t i = 0; i < angles.size(); i++) {
            auto angle = angles[i];
            image_to_angle.insert(make_pair(i, angle));
        }
        result.insert(make_pair(height, image_to_angle));
    }
    return result;
}

void accumulate_azimuth(const model_ptr& model,
                        const rcs_data& rcs_data,
                        const string& input_path,
                        const string& output_path) {
    auto world_scale = get_world_scale(CAMERA_DISTANCE, *model);
    auto time_measure = start_time();
    auto height_to_image_to_angle = get_height_map(rcs_data);
    log_and_start_next(time_measure, "Constructing map of heights to images to azimuth angles");

    auto ranges = rcs_data.at_height(DEFAULT_HEIGHT)->ranges();
    auto first_range = static_cast<double>(ranges[0]);
    auto last_range = static_cast<double>(ranges[ranges.size() - 1]);

    rcs_sums(model, rcs_data,
             [rcs_data, height_to_image_to_angle, world_scale, first_range, last_range]
                     (const rcs_height_t height,
                      const size_t image_index,
                      const relative_point& point) {
                 auto point_distance = point.distance / world_scale;
                 if (point_distance > last_range + RANGE_EPSILON || point_distance < first_range - RANGE_EPSILON) {
                     return 0.0;
                 }

                 auto rcs_row = rcs_data.at_height(height);
                 auto azimuth_for_image = height_to_image_to_angle.at(height).at(image_index);
                 auto azimuth_values = rcs_row->azimuth()[azimuth_for_image];
                 auto azimuth_data = find_azimuth(point_distance, azimuth_values, first_range);
                 return azimuth_data;
             }, input_path, output_path);
}