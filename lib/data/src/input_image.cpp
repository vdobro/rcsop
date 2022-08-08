#include "input_image.h"

#include <stdexcept>
#include <regex>
#include <utility>

namespace rcsop::data {
    using std::regex;
    using std::smatch;
    using std::invalid_argument;

    using rcsop::common::height_t;

    static const regex fallback_folder_name_pattern("^(\\d{1,3})(cm)?$");
    static const regex fallback_image_name_pattern("^(\\d{3})°.*\\.png$");

    static const regex image_filename_pattern("^.*(\\d{1,3})cm_(\\d{3})°\\.png$");

    CameraInputImage::CameraInputImage(const path& file_path, path image_root)
            : _file_path(file_path),
              _image_root_folder(std::move(image_root)),
              _camera_position(std::nullopt) {
        this->_camera_position = parse_position_from_file_path(file_path);
    }

    optional<ObserverPosition> CameraInputImage::position() const {
        return this->_camera_position;
    }

    path CameraInputImage::file_path() const {
        return this->_file_path;
    }

    string CameraInputImage::image_name() const {
        return std::filesystem::relative(this->_file_path, this->_image_root_folder).string();
    }

    optional<azimuth_t> CameraInputImage::parse_angle_from_name(const string& filename) {
        const optional<ObserverPosition> filename_result = parse_position_from_name(filename);
        if (filename_result.has_value()) {
            return filename_result.value().azimuth;
        }
        smatch filename_sm;
        if (!regex_match(filename, filename_sm, fallback_image_name_pattern)) {
            return std::nullopt;
        }
        return stoi(filename_sm[1]);
    }

    optional<ObserverPosition> CameraInputImage::parse_position_from_name(const string& filename) {
        smatch filename_sm;

        height_t height;
        azimuth_t angle;
        if (!regex_match(filename, filename_sm, image_filename_pattern)) {
            return std::nullopt;
        }

        height = stoi(filename_sm[1]);
        angle = stoi(filename_sm[2]);

        return ObserverPosition{
                .height = height,
                .azimuth = angle,
        };
    }

    optional<ObserverPosition> CameraInputImage::parse_position_from_file_path(const path& path) {
        const string filename = path.filename().string();
        const string parent_folder_name = path.parent_path().filename().string();

        const optional<ObserverPosition> filename_result = parse_position_from_name(filename);
        if (filename_result.has_value()) {
            return filename_result;
        }
        const optional<azimuth_t> angle = parse_angle_from_name(filename);
        if (!angle.has_value()) {
            return std::nullopt;
        }
        smatch parent_folder_sm;
        if (!regex_match(parent_folder_name, parent_folder_sm, fallback_folder_name_pattern)) {
            return std::nullopt;
        }

        const height_t height = stoi(parent_folder_sm[1]);
        return ObserverPosition{
                .height = height,
                .azimuth = angle.value(),
        };
    }
}
