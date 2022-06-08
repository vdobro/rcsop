#include "cleanup.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

const char* PARAM_INPUT_PATH = "input-path";
const char* PARAM_OUTPUT_PATH = "output-path";

int main(int argc, char *argv[]) {
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
    auto model = std::make_shared<SparseCloud>(input_path);

    const path output_path{vm.at(PARAM_OUTPUT_PATH).as<string>()};
    filter_points(*model, output_path);

    return EXIT_SUCCESS;
}
