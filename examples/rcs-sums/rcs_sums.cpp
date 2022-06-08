#include "rcs_sums.h"

#include "utils/chronometer.h"

#define HORIZONTAL_ANGLE 25
#define VERTICAL_ANGLE 15
#define STANDARD_DEVIATION 0.2

#define HORIZONTAL_SPREAD (HORIZONTAL_ANGLE / 2.0)
#define VERTICAL_SPREAD (VERTICAL_ANGLE / 2.0)

static const double RANGE_EPSILON = 3;

using std::string;
using std::make_pair;

typedef struct gauss_options {
    double sigma;
    double integral_factor;
    double x_scale;
    double y_scale;
} gauss_options;

using namespace sfm::rendering;

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

static void rcs_sums(const SparseCloud& model,
                     const rcs_data& rcs_data,
                     const std::function<double(rcs_height_t, size_t, const relative_point&)>& rcs_mapper,
                     const path& input_path,
                     const path& output_path) {

    auto images = model.get_cameras();
#ifdef SINGLE_PROJECTION
    size_t first_image = DEFAULT_CAMERA;
    size_t last_image_plus_one = DEFAULT_CAMERA + 1;
    auto heights = vector<long>{DEFAULT_HEIGHT};
#else
    size_t first_image = 0;
    size_t last_image_plus_one = images.size();
    auto heights = rcs_data.available_heights();
#endif
    auto world_scale = model.get_world_scale(CAMERA_DISTANCE);
    auto scored_points = model.get_scored_points();

    const gauss_options distribution_options = {
            .sigma = STANDARD_DEVIATION,
            .integral_factor = get_gauss_integral_factor(STANDARD_DEVIATION),
            .x_scale = HORIZONTAL_SPREAD,
            .y_scale = VERTICAL_SPREAD,
    };
    auto time_measure = start_time();
    for (size_t image_index = first_image;
         image_index < last_image_plus_one;
         image_index++) {
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

    const path render_path{output_path / "render"};
    std::filesystem::remove_all(render_path);

    auto colormap = get_colormap(filtered_points, COLOR_MAP);
    render_images(model, input_path, render_path, filtered_points, colormap);

    log_and_start_next(time_measure, "(Total) rendering done.");
}

void accumulate_rcs(const SparseCloud& model,
                    const rcs_data& rcs_data,
                    const path& input_path,
                    const path& output_path) {
    rcs_sums(model, rcs_data, [rcs_data](rcs_height_t height, size_t image_index, const relative_point& point) {
        return rcs_data.at_height(height)->rcs()[image_index];
    }, input_path, output_path);
}

static double find_azimuth(double real_distance,
                           const vector<double>& azimuth_values,
                           const double first) {
    return find_interval_match(real_distance, azimuth_values, first, RANGE_EPSILON);
}

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

void accumulate_azimuth(const SparseCloud& model,
                        const rcs_data& rcs_data,
                        const path& input_path,
                        const path& output_path) {
    auto world_scale = model.get_world_scale(CAMERA_DISTANCE);
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
