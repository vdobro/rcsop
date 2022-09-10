#include "launcher_options.h"

#include <iostream>
#include <chrono>
#include "boost/program_options.hpp"

#include "utils/task_utils.h"

#include "default_options.h"

namespace rcsop::launcher {
    using rcsop::launcher::utils::PointGenerator;

    namespace po = boost::program_options;
    using std::chrono::system_clock;
    using std::cout;

    static const char* PARAM_INPUT_PATH = "input-path";
    static const char* PARAM_TASK = "task";
    static const char* PARAM_OUTPUT_PATH = "output-path";
    static const char* PARAM_OUTPUT_NAME_NO_TIMESTAMP = "no-timestamp";
    static const char* PARAM_SOFTWARE_RENDERING = "software-rendering";
    static const char* PARAM_CAMERA_DISTANCE = "camera-distance";
    static const char* PARAM_USE_CLOSEST_CAMERA = "use-closest-camera";
    static const char* PARAM_NO_DATA_PREFILTER = "no-data-prefilter";
    static const char* PARAM_PITCH_CORRECTION = "pitch-correction";
    static const char* PARAM_DEFAULT_HEIGHT = "default-height";
    static const char* PARAM_DB_MIN = "db-min";
    static const char* PARAM_DB_MAX = "db-max";
    static const char* PARAM_VERTICAL_ANGLE_SPREAD = "vertical-spread";
    static const char* PARAM_VERTICAL_DISTRIBUTION_VARIANCE = "vertical-variance";
    static const char* PARAM_POINT_GENERATOR = "points";
    static const char* PARAM_GRADIENT_RADIUS = "gradient-radius";
    static const char* PARAM_COLOR_MAP = "color-map";
    static const char* PARAM_ALPHA = "alpha";

    [[nodiscard]] static string get_current_timestamp() {
        const auto now = system_clock::now();
        const time_t t = system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%FT%T");
        return ss.str();
    }

    static void validate_main_task(const path& input_path,
                                   const path& output_path,
                                   const string& task,
                                   const map<string, launcher_task>& available_tasks) {
        if (!available_tasks.contains(task)) {
            throw invalid_argument(
                    "Could not find task '" + task + "', consult usage documentation for available options.");
        }
        if (!is_directory(input_path)) {
            throw invalid_argument("Input directory does not exist.");
        }
        if (is_regular_file(output_path)) {
            throw invalid_argument("Output path not a folder and already exists as a file.");
        }
    }

    static void validate_task_options(const task_options& options) {
        if (options.camera.distance_to_origin <= 0.) {
            throw invalid_argument("Camera distance must be greater than zero.");
        }
        if (options.camera.default_height <= 0) {
            throw invalid_argument("Default camera height must be non-negative.");
        }
        auto gradient_alpha = options.rendering.gradient.center_alpha;
        if (gradient_alpha > 1. || gradient_alpha <= 0.) {
            throw invalid_argument("Gradient center alpha must be in range (0, 1].");
        }
        if (options.rendering.gradient.radius <= 0) {
            throw invalid_argument("Gradient radius must be larger than 0.");
        }
        if (options.db_range.min >= options.db_range.max) {
            throw invalid_argument("dB range must be well defined: lower bound smaller than the upper bound.");
        }
        if (options.vertical_options.angle_spread <= 0) {
            throw invalid_argument("No data to display with a negative vertical angle spread.");
        }
        if (options.vertical_options.normal_variance == 0) {
            throw invalid_argument("Vertical spread variance must not be 0.");
        }
    }

    static auto parse_point_generator_option(const string& option) -> PointGenerator {
        if (option == "model-cloud") {
            return PointGenerator::MODEL_POINT_CLOUD;
        }
        if (option == "bounding-box") {
            return PointGenerator::BOUNDING_BOX;
        }
        if (option == DEFAULT_POINT_GENERATOR) {
            return PointGenerator::DATA_PROJECTION;
        }
        if (option == "model-with-projection") {
            return PointGenerator::MODEL_WITH_PROJECTION;
        }
        throw invalid_argument(string(PARAM_POINT_GENERATOR)
                               + " must be one of the following: model-cloud, bounding-box, data-projection or model-with-projection");
    }

    [[nodiscard]] po::variables_map parse_arguments(int argc, char* argv[]) {
        po::options_description desc("RCSOP, Copyright 2022 FH Aachen");
        desc.add_options()
                ("input-path,I", po::value<string>(),
                 "folder with the inputs: source images, sparse/dense point clouds and MATLAB data sources")
                ("output-path,O", po::value<string>(), "output path")
                ("task,T", po::value<string>()->default_value(DEFAULT_TASK), "task to execute")
                (PARAM_OUTPUT_NAME_NO_TIMESTAMP, po::bool_switch(),
                 "use current timestamp as output folder name")
                (PARAM_SOFTWARE_RENDERING, po::bool_switch(),
                 "enable software rendering instead of GPU")
                ("camera-distance,R", po::value<double>()->default_value(DEFAULT_CAMERA_DISTANCE),
                 "distance from the camera to the origin/center in centimeters")
                (PARAM_USE_CLOSEST_CAMERA, po::bool_switch(),
                 "use any closest camera if none found at the same angle")
                (PARAM_NO_DATA_PREFILTER, po::bool_switch(),
                 "filter RCS data to only contain one maximum value per azimuth angle")
                (PARAM_PITCH_CORRECTION, po::value<double>()->default_value(DEFAULT_CAMERA_PITCH_CORRECTION),
                 "camera pitch correction")
                (PARAM_DEFAULT_HEIGHT, po::value<height_t>()->default_value(DEFAULT_HEIGHT),
                 "default camera height, used if not specified in point cloud data or source image names")
                (PARAM_DB_MIN, po::value<double>()->default_value(DEFAULT_MIN_DB),
                 "lower bound of the decibel range")
                (PARAM_DB_MAX, po::value<double>()->default_value(DEFAULT_MAX_DB),
                 "upper bound of the decibel range")
                (PARAM_GRADIENT_RADIUS, po::value<float>()->default_value(DEFAULT_GRADIENT_RADIUS),
                 "gradient radius in points")
                (PARAM_VERTICAL_ANGLE_SPREAD, po::value<double>()->default_value(DEFAULT_VERTICAL_ANGLE_SPREAD),
                 "Maximum vertical spread applied to point clouds, both projected and observed")
                (PARAM_VERTICAL_DISTRIBUTION_VARIANCE, po::value<double>()->default_value(DEFAULT_VERTICAL_DISTRIBUTION_VARIANCE),
                 "variance (sigma squared) for the vertical distribution of dB values, defaults to Stephen Stigler's definition")
                (PARAM_POINT_GENERATOR, po::value<string>()->default_value(DEFAULT_POINT_GENERATOR),
                 "use the legacy point generator (only filtering a bounding box or using the sparse cloud model)")
                (PARAM_COLOR_MAP, po::value<string>()->default_value(DEFAULT_COLOR_MAP),
                 "default color map to use")
                (PARAM_ALPHA, po::value<float>()->default_value(DEFAULT_ALPHA),
                 "base alpha value/factor to apply and full color intensity");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (!vm.count(PARAM_INPUT_PATH) || !vm.count(PARAM_OUTPUT_PATH)) {
            cout << desc;
            exit(EXIT_FAILURE);
        }
        return vm;
    }

    task_options parse_and_validate(int argc, char* argv[],
                                    const map<string, launcher_task>& available_tasks) {
        auto vm = parse_arguments(argc, argv);

        const path input_path{vm.at(PARAM_INPUT_PATH).as<string>()};
        const path output_path{vm.at(PARAM_OUTPUT_PATH).as<string>()};
        const string task{vm.at(PARAM_TASK).as<string>()};
        validate_main_task(input_path, output_path, task, available_tasks);

        path task_output_path{output_path / task};
        const bool use_timestamps_as_output_name = !vm.at(PARAM_OUTPUT_NAME_NO_TIMESTAMP).as<bool>();
        if (use_timestamps_as_output_name) {
            auto output_target_folder = get_current_timestamp();
            task_output_path = output_path / task / output_target_folder;
        }

        const double camera_distance{vm.at(PARAM_CAMERA_DISTANCE).as<double>()};
        const height_t default_camera_height{vm.at(PARAM_DEFAULT_HEIGHT).as<height_t>()};
        const float base_alpha = vm.at(PARAM_ALPHA).as<float>();
        const double min_db = vm.at(PARAM_DB_MIN).as<double>();
        const double max_db = vm.at(PARAM_DB_MAX).as<double>();
        const double pitch_correction{vm.at(PARAM_PITCH_CORRECTION).as<double>()};
        const float gradient_radius = vm.at(PARAM_GRADIENT_RADIUS).as<float>();
        const bool use_software_rendering = vm.at(PARAM_SOFTWARE_RENDERING).as<bool>();
        const auto color_map = rcsop::common::coloring::resolve_map_by_name(vm.at(PARAM_COLOR_MAP).as<string>());
        const bool filter_data = !vm.at(PARAM_NO_DATA_PREFILTER).as<bool>();
        const double vertical_spread = vm.at(PARAM_VERTICAL_ANGLE_SPREAD).as<double>();
        const double vertical_distribution_variance = sqrt(abs(vm.at(PARAM_VERTICAL_DISTRIBUTION_VARIANCE).as<double>()));
        const bool use_any_closest_observer = vm.at(PARAM_USE_CLOSEST_CAMERA).as<bool>();
        const PointGenerator point_generator = vm.contains(PARAM_POINT_GENERATOR)
                ? parse_point_generator_option(vm.at(PARAM_POINT_GENERATOR).as<string>())
                : PointGenerator::DATA_PROJECTION; // default

        task_options options{
                .task_name = task,
                .input_path = input_path,
                .output_path = task_output_path,
                .prefilter_data = filter_data,
                .vertical_options = {
                        .angle_spread = vertical_spread,
                        .normal_variance = vertical_distribution_variance,
                },
                .point_generator = point_generator,
                .db_range = {
                        .min = min_db,
                        .max = max_db,
                },
                .camera = {
                        .pitch_correction = pitch_correction,
                        .distance_to_origin = camera_distance,
                        .default_height = default_camera_height,
                        .use_any_camera_nearby = use_any_closest_observer,
                },
                .rendering = {
                        .use_gpu_rendering = !use_software_rendering,
                        .color_map = color_map,
                        .gradient = {
                                .radius = gradient_radius,
                                .center_alpha = base_alpha,
                        },
                },
        };
        validate_task_options(options);
        return options;
    }
}