#include "input_image.h"

#include <stdexcept>
#include <regex>

using std::regex;
using std::smatch;
using std::invalid_argument;

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
    if (!regex_match(filename_string, filename_sm, image_filename_pattern)) {
        return std::nullopt;
    }
    const height_t height = stoi(filename_sm[1]);
    const azimuth_t angle = stoi(filename_sm[2]);
    return ObserverPosition{
            .height = height,
            .azimuth = angle,
    };
}
