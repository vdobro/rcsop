#include <filesystem>
#include <iostream>
#include <string>

#include "boost/program_options.hpp"

#include "filter-ground-plane.h"
#include "filter-outliers.h"

namespace po = boost::program_options;

using std::string;

using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::remove;

const char* PARAM_INPUT_PATH = "input-path";
const char* PARAM_OUTPUT_PATH = "output-path";

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("input-path,I", po::value<string>(), "path with the COLMAP model to be trimmed")
            ("output-path,O", po::value<string>(), "output path");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (!vm.count(PARAM_INPUT_PATH) || !vm.count(PARAM_OUTPUT_PATH)) {
        std::cout << desc;
        exit(1);
    }
    const path input_path{vm.at(PARAM_INPUT_PATH).as<string>()};
    const path output_path{vm.at(PARAM_OUTPUT_PATH).as<string>()};

    create_directories(output_path.parent_path());
    if (std::filesystem::is_regular_file(output_path)) {
        remove(output_path);
    }

    const path intermediate_path{output_path.parent_path() / "intermediate.ply"};
    if (std::filesystem::is_regular_file(intermediate_path)) {
        remove(intermediate_path);
    }

    separate_plane(input_path, intermediate_path);
    filter_outliers(intermediate_path, output_path);

    if (std::filesystem::is_regular_file(intermediate_path)) {
        remove(intermediate_path);
    }

    return EXIT_SUCCESS;
}
