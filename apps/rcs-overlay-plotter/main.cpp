#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "boost/program_options.hpp"

#include "input_data_collector.h"
#include "test_task.h"

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

const map<string, std::function<void(const InputDataCollector&,
        const path&)>> available_tasks = {
        {"test-task", dummy_task}
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

    const string current_timestamp = get_current_timestamp();
    const path task_output_path{output_path / task / current_timestamp};

    if (is_directory(task_output_path)) {
        cerr << "Warning: a directory with the name '" << task_output_path
             << "' exists already and will be removed" << std::endl;
        remove_all(task_output_path);
    }
    create_directories(task_output_path);

    shared_ptr<InputDataCollector> input_collector = make_shared<InputDataCollector>(input_path);

    const auto task_executor = available_tasks.at(task);
    task_executor(*input_collector, task_output_path);

    return EXIT_SUCCESS;
}
