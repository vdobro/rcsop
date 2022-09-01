#include "input_image.h"

#include <stdexcept>
#include <utility>

namespace rcsop::data {
    using std::invalid_argument;

    using rcsop::common::height_t;

    static const regex fallback_folder_name_pattern("^(\\d{2})(cm)?$");
    static const size_t FALLBACK_PARENT_FOLDER_HEIGHT = 1;

    static const regex fallback_image_name_pattern("^(\\d{3})°.*\\.png$");
    static const size_t FALLBACK_IMAGE_FILENAME_ANGLE = 1;

    static const regex image_filename_pattern("^(.*_)?(\\d{2})cm_(\\d{3})°\\.png$");
    static const size_t IMAGE_FILENAME_HEIGHT = 2;
    static const size_t IMAGE_FILENAME_AZIMUTH = 3;

    CameraInputImage::CameraInputImage(const path& file_path,
                                       path image_root,
                                       height_t default_height)
            : _file_path(file_path),
              _image_root_folder(std::move(image_root)),
              _camera_position(std::nullopt) {
        this->_camera_position = parse_position_from_file_path(file_path, default_height);
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

    optional<ObserverPosition> CameraInputImage::parse_position_from_name(const string& filename) {
        smatch filename_sm;

        height_t height;
        azimuth_t azimuth;
        if (!regex_match(filename, filename_sm, image_filename_pattern)) {
            return std::nullopt;
        }

        height = stoi(filename_sm[IMAGE_FILENAME_HEIGHT]);
        azimuth = stoi(filename_sm[IMAGE_FILENAME_AZIMUTH]);

        return ObserverPosition{
                .height = height,
                .azimuth = azimuth,
        };
    }

    optional<azimuth_t> CameraInputImage::parse_angle_from_name(const string& filename) {
        const optional<ObserverPosition> full_position = parse_position_from_name(filename);
        if (full_position.has_value()) {
            return full_position.value().azimuth;
        }
        smatch fallback_filename_sm;
        if (!regex_match(filename, fallback_filename_sm, fallback_image_name_pattern)) {
            return std::nullopt;
        }
        return stoi(fallback_filename_sm[FALLBACK_IMAGE_FILENAME_ANGLE]);
    }

    optional<ObserverPosition> CameraInputImage::parse_position_from_file_path(
            const path& path,
            height_t default_height) {

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
            return ObserverPosition{
                    .height = default_height,
                    .azimuth = angle.value(),
            };
        }

        const height_t height = stoi(parent_folder_sm[FALLBACK_PARENT_FOLDER_HEIGHT]);
        return ObserverPosition{
                .height = height,
                .azimuth = angle.value(),
        };
    }
}
