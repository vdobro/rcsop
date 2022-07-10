#include "launcher.h"

#include <iostream>
#include <chrono>

#include "boost/program_options.hpp"

#include "utils/types.h"
#include "utils/task_utils.h"

#include "tasks/test_task.h"
#include "tasks/rcs_slices.h"
#include "tasks/rcs_sums.h"
#include "tasks/azimuth_rcs_plotter.h"
#include "tasks/sparse_filter.h"

#include "default_options.h"

namespace po = boost::program_options;

using std::stringstream;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;

using std::filesystem::remove_all;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

using std::chrono::system_clock;

const char* PARAM_INPUT_PATH = "input-path";
const char* PARAM_TASK = "task";
const char* PARAM_OUTPUT_PATH = "output-path";
const char* PARAM_OUTPUT_NAME_TIMESTAMP = "timestamp";
const char* PARAM_SOFTWARE_RENDERING = "software-rendering";
const char* PARAM_CAMERA_DISTANCE = "camera-distance";
const char* PARAM_PITCH_CORRECTION = "pitch-correction";
const char* PARAM_DEFAULT_HEIGHT = "default-height";

using namespace rcsop::launcher::tasks;
const static char* DEFAULT_TASK = "azimuth-rcs";
const static map<string, rcsop::launcher::utils::launcher_task> available_tasks = {
        {"test-task",     test_task},
        {"rcs-slices",    rcs_slices},
        {"rcs-sums",      accumulate_rcs},
        {"azimuth-sums",  accumulate_azimuth},
        {DEFAULT_TASK,    azimuth_rcs_plotter},
        {"sparse-filter", sparse_filter},
};

static string get_current_timestamp() {
    const auto now = system_clock::now();
    const time_t t = system_clock::to_time_t(now);
    stringstream ss;
    ss << std::put_time(std::localtime(&t), "%FT%T");
    return ss.str();
}

static void validate_options(const path& input_path,
                             const path& output_path,
                             const string& task,
                             const double camera_distance,
                             const height_t default_height) {
    if (!available_tasks.contains(task)) {
        cerr << "Could not find task '" << task << "', consult documentation for available options" << endl;
        exit(EXIT_FAILURE);
    }
    if (!is_directory(input_path)) {
        cerr << "Input directory does not exist, exiting." << endl;
        exit(EXIT_FAILURE);
    }
    if (is_regular_file(output_path)) {
        cerr << "Output path not a folder and already exists as a file, exiting." << endl;
        exit(EXIT_FAILURE);
    }
    if (camera_distance <= 0.) {
        cerr << "Camera distance must be greater than zero" << endl;
        exit(EXIT_FAILURE);
    }
    if (default_height <= 0) {
        cerr << "Default camera height must be non-negative" << endl;
        exit(EXIT_FAILURE);
    }
}

using rcsop::launcher::utils::task_options;

int rcsop::launcher::launcher_main(int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("input-path,I", po::value<string>(),
             "folder with the inputs: source images, sparse/dense point clouds and MATLAB data sources")
            ("output-path,O", po::value<string>(), "output path")
            ("task,T", po::value<string>()->default_value(DEFAULT_TASK), "task to execute")
            (PARAM_OUTPUT_NAME_TIMESTAMP, po::bool_switch()->default_value(true),
             "use current timestamp as output folder name")
            (PARAM_SOFTWARE_RENDERING, po::bool_switch()->default_value(false),
             "enable software rendering instead of GPU")
            ("camera-distance,R", po::value<double>()->default_value(DEFAULT_CAMERA_DISTANCE),
             "distance from the camera to the origin/center in centimeters")
            (PARAM_PITCH_CORRECTION, po::value<double>()->default_value(DEFAULT_CAMERA_PITCH_CORRECTION),
             "camera pitch correction")
            (PARAM_DEFAULT_HEIGHT, po::value<height_t>()->default_value(DEFAULT_HEIGHT),
             "default camera height, used if not specified in point cloud data or source image names");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (!vm.count(PARAM_INPUT_PATH) || !vm.count(PARAM_OUTPUT_PATH)) {
        cout << desc;
        exit(EXIT_FAILURE);
    }

    const path input_path{vm.at(PARAM_INPUT_PATH).as<string>()};
    const path output_path{vm.at(PARAM_OUTPUT_PATH).as<string>()};
    const string task{vm.at(PARAM_TASK).as<string>()};
    const double camera_distance{vm.at(PARAM_CAMERA_DISTANCE).as<double>()};
    const height_t default_camera_height{vm.at(PARAM_DEFAULT_HEIGHT).as<height_t>()};
    validate_options(input_path, output_path, task, camera_distance, default_camera_height);

    const double pitch_correction{vm.at(PARAM_PITCH_CORRECTION).as<double>()};

    bool use_timestamps_as_output_name = vm.at(PARAM_OUTPUT_NAME_TIMESTAMP).as<bool>();
    bool use_software_rendering = vm.at(PARAM_SOFTWARE_RENDERING).as<bool>();

    string output_target_folder = "latest";
    if (use_timestamps_as_output_name) {
        output_target_folder = get_current_timestamp();
    }
    const path task_output_path{output_path / task / output_target_folder};

    if (is_directory(task_output_path)) {
        cerr << "Warning: a directory with the name '" << task_output_path.string()
             << "' exists already and will be removed" << endl;
        remove_all(task_output_path);
    }
    create_directories(task_output_path);

    const task_options options{
            .input_path = input_path,
            .output_path = task_output_path,
            .db_range = {
                    .min = -20,
                    .max = 5,
            },
            .camera = {
                    .pitch_correction = pitch_correction,
                    .distance_to_origin = camera_distance,
                    .default_height = default_camera_height,
            },
            .rendering = {
                    .use_gpu_rendering = !use_software_rendering,
                    .color_map = COLOR_MAP,
                    .gradient = {
                            .radius = 25.f,
                            .center_alpha = 0.3f,
                    },
            },
    };

    try {
        InputDataCollector input_collector(input_path);

        const auto task_executor = available_tasks.at(task);
        task_executor(input_collector, options);
    } catch (const std::exception& e) {
        cerr << "Failed to execute given task, reason:" << endl;
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}