#include "input_image.h"

#include <stdexcept>
#include <regex>

using std::regex;
using std::smatch;
using std::invalid_argument;

static const regex fallback_folder_name_pattern("^(\\d{1,3})(cm)?$");
static const regex fallback_image_name_pattern("^(\\d{3})°.*\\.png$");

static const regex image_filename_pattern("^(\\d{1,3})cm_(\\d{3})°\\.png$");

CameraInputImage::CameraInputImage(const path& path)
        : _file_path(path),
          _camera_position(std::nullopt) {
    this->_camera_position = parse_filename(path);
}

std::optional<ObserverPosition> CameraInputImage::position() const {
    return this->_camera_position;
}

path CameraInputImage::file_path() const {
    return this->_file_path;
}

std::optional<ObserverPosition> CameraInputImage::parse_filename(const path& path) {
    auto filename_string = path.filename().string();
    smatch filename_sm;

    height_t height;
    azimuth_t angle;
    if (regex_match(filename_string, filename_sm, image_filename_pattern)) {
        height = stoi(filename_sm[1]);
        angle = stoi(filename_sm[2]);
    } else {
        auto parent_string = path.parent_path().filename().string();
        smatch parent_folder_sm;
        smatch fallback_filename_sm;
        if (regex_match(parent_string, parent_folder_sm, fallback_folder_name_pattern)
            && regex_match(filename_string, fallback_filename_sm, fallback_image_name_pattern)) {
            height = stoi(parent_folder_sm[1]);
            angle = stoi(fallback_filename_sm[1]);
        } else {
            return std::nullopt;
        }
    }
    return ObserverPosition{
            .height = height,
            .azimuth = angle,
    };
}
