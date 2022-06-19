#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "boost/program_options.hpp"
#include "input_data_collector.h"

#include "tasks/test_task.h"
#include "tasks/azimuth_rcs_plotter.h"
#include "tasks/rcs_slices.h"

namespace po = boost::program_options;

using std::string;
using std::vector;
using std::map;
using std::function;
using std::stringstream;
using std::cout;
using std::cerr;
using std::clog;
using std::endl;
using std::make_shared;

using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::remove;
using std::filesystem::remove_all;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

using std::chrono::system_clock;

const char* PARAM_INPUT_PATH = "input-path";
const char* PARAM_TASK = "task";
const char* PARAM_OUTPUT_PATH = "output-path";
const char* PARAM_OUTPUT_NAME_TIMESTAMP = "timestamp";

const map<string, std::function<void(const shared_ptr<InputDataCollector>,
                                     const path&)>> available_tasks = {
        {"test-task", dummy_task},
        {"azimuth-rcs", azimuth_rcs_plotter},
        {"rcs-slices", rcs_slices},
};

string get_current_timestamp() {
    const auto now = system_clock::now();
    const time_t t = system_clock::to_time_t(now);
    stringstream ss;
    ss << std::put_time(std::localtime(&t), "%FT%T");
    return ss.str();
}

void validate_options(const path& input_path,
                      const path& output_path,
                      const string& task) {
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
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("input-path,I", po::value<string>(),
             "folder with the inputs: source images, sparse/dense point clouds and MATLAB data sources")
            ("task,T", po::value<string>(), "task to execute")
            ("timestamp,M", po::value<bool>(), "whether output folder name should be the current timestamp")
            ("output-path,O", po::value<string>(), "output path");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (!vm.count(PARAM_INPUT_PATH) || !vm.count(PARAM_OUTPUT_PATH) || !vm.count(PARAM_TASK)) {
        cout << desc;
        exit(EXIT_FAILURE);
    }

    const path input_path{vm.at(PARAM_INPUT_PATH).as<string>()};
    const path output_path{vm.at(PARAM_OUTPUT_PATH).as<string>()};
    const string task{vm.at(PARAM_TASK).as<string>()};
    validate_options(input_path, output_path, task);

    bool use_timestamps_as_output_name = true;
    if (vm.count(PARAM_OUTPUT_NAME_TIMESTAMP)) {
        use_timestamps_as_output_name = vm.at(PARAM_OUTPUT_NAME_TIMESTAMP).as<bool>();
    }

    string output_target_folder = "latest";
    if (use_timestamps_as_output_name){
        output_target_folder = get_current_timestamp();
    }
    const path task_output_path{output_path / task / output_target_folder};

    if (is_directory(task_output_path)) {
        cerr << "Warning: a directory with the name '" << task_output_path.string()
             << "' exists already and will be removed" << endl;
        remove_all(task_output_path);
    }
    create_directories(task_output_path);

    try {
        shared_ptr<InputDataCollector> input_collector = make_shared<InputDataCollector>(input_path);

        const auto task_executor = available_tasks.at(task);
        task_executor(input_collector, task_output_path);
    } catch (const std::exception& e) {
        cerr << "Failed to execute given task, reason:" << endl;
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
