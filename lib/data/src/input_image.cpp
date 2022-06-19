#include "input_image.h"

#include <stdexcept>
#include <regex>

using std::regex;
using std::smatch;
using std::invalid_argument;

static const regex image_filename_pattern("^(\\d{3})°.*\\.png$");
static const regex image_folder_name_pattern("^(\\d{1,3})cm$");

const height_t DEFAULT_HEIGHT = 40;

CameraInputImage::CameraInputImage(const path& path) : _file_path(path), _camera_position(ObserverPosition{
        .height = DEFAULT_HEIGHT,
        .azimuth = 0,
}){
    auto filename_string = path.filename().string();
    smatch filename_sm;
    if (!regex_match(filename_string, filename_sm, image_filename_pattern)) {
        throw invalid_argument("Could not parse image name: " + filename_string);
    }
    const azimuth_t angle = stoi(filename_sm[1]);

    auto folder_path_string = path.parent_path().filename().string();
    smatch folder_sm;
    if (!regex_match(folder_path_string, folder_sm, image_folder_name_pattern)) {
        throw invalid_argument("Folder containing image did not indicate its height as position: " + std::to_string(DEFAULT_HEIGHT) + " cm");
    }
    const height_t height = stoi(folder_sm[1]);
    this->_camera_position = {
            .height = height,
            .azimuth = angle,
    };
}

ObserverPosition CameraInputImage::position() const {
    return this->_camera_position;
}

path CameraInputImage::file_path() const {
    return this->_file_path;
}
