#include "cleanup.h"

int main() {
    const path data_root_path{"data"};
    const path input_path{data_root_path / "input"};
    const path input_image_path{input_path / "audi_40"};

    const path output_path{data_root_path / "output"};

    const path model_path{input_path / "model"};
    auto model = std::make_shared<sparse_cloud>(model_path);

    const path filtered_model_path{input_path / "filtered_model"};
    filter_points(*model, filtered_model_path);

    return EXIT_SUCCESS;
}
