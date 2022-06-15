#include "input_image.h"

#include <regex>

#include "azimuth_rcs_map.h"

using std::regex;
using std::smatch;

static const regex image_filename_pattern("^(\\d{3})°.*\\.png$");
static const regex image_folder_name_pattern("^(\\d{1,3})cm$");

const height_t DEFAULT_HEIGHT = 40;

InputImage::InputImage(const path& path) {
    this->_file_path = path;

    auto filename_string = path.filename().string();
    smatch filename_sm;
    if (!regex_match(filename_string, filename_sm, image_filename_pattern)) {
        std::cerr << "Could not parse image name: " << filename_string << std::endl;
        throw invalid_argument("path");
    }
    const azimuth_t angle = stoi(filename_sm[1]);

    auto folder_path_string = path.parent_path().filename().string();
    height_t height;
    smatch folder_sm;
    if (regex_match(folder_path_string, folder_sm, image_folder_name_pattern)) {
        height = stoi(folder_sm[1]);
    } else {
        std::cerr << "Warning: Folder containing image did not indicate its image_height, assuming default: "
                  << std::to_string(DEFAULT_HEIGHT)
                  << " cm"
                  << std::endl;
        height = DEFAULT_HEIGHT;
    }
    this->_camera_position = {
            .height = height,
            .azimuth = angle,
    };
}

ObserverPosition InputImage::position() const {
    return this->_camera_position;
}

path InputImage::file_path() const {
    return this->_file_path;
}

azimuth_t InputImage::azimuth() const {
    return this->_camera_position.azimuth;
}
