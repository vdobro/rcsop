#include "rcs_data.h"
#include "slices.h"

int main() {

    const path data_root_path{"data"};
    const path input_path{data_root_path / "input"};
    const path input_image_path{input_path / "audi_40"};

    const path output_path{data_root_path / "output"};

    const path model_path{input_path / "filtered_model"};
    auto model = std::make_shared<sparse_cloud>(model_path);

    const path rcs_file_path{input_path / "rcs.mat"};
    const auto rcs_file = rcs_data(rcs_file_path);
    const auto rcs = rcs_file.at_height(40)->rcs();
    
    const path slice_output_path{output_path / "colored_slices"};
    color_slices(*model, rcs, input_image_path, slice_output_path);

    return EXIT_SUCCESS;
}
