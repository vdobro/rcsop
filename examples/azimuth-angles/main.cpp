#include "rcs_data.h"

#include "azimuth_angles.h"

int main() {
    const path data_root_path{"datasets"};
    const path input_path{data_root_path / "input"};
    const path input_image_path{input_path / "audi_40"};

    const path output_path{data_root_path / "output"};

    const path model_path{input_path / "filtered_model"};
    auto model = std::make_shared<sparse_cloud>(model_path);

    const path rcs_file_path{input_path / "rcs.mat"};
    const auto rcs_file = rcs_data(rcs_file_path);
    const auto rcs = rcs_file.at_height(40)->rcs();

    const path azimuth_angle_data_path{input_path / "AudiAuswertung"};
    const path output_path_azimuth{output_path / "azimuth_angles"};

    auto azimuth_data = display_azimuth(model, input_image_path, azimuth_angle_data_path, output_path_azimuth);
    render_to_files(*azimuth_data, output_path_azimuth);

    return EXIT_SUCCESS;
}