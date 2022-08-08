#ifndef RCSOP_DATA_INPUT_IMAGE_H
#define RCSOP_DATA_INPUT_IMAGE_H

#include "utils/types.h"
#include "observer_position.h"

namespace rcsop::data {
    using rcsop::common::ObserverPosition;
    using rcsop::common::azimuth_t;
    using rcsop::common::height_t;

    class CameraInputImage {
    private:
        path _file_path;
        path _image_root_folder;
        optional<ObserverPosition> _camera_position;

    public:
        explicit CameraInputImage(const path& file_path, path image_root, height_t default_height);

        [[nodiscard]] optional<ObserverPosition> position() const;

        [[nodiscard]] path file_path() const;

        [[nodiscard]] string image_name() const;

        [[nodiscard]] static optional<ObserverPosition>
        parse_position_from_file_path(const path& path, height_t default_height);

        [[nodiscard]] static optional<ObserverPosition> parse_position_from_name(const string& filename);

        [[nodiscard]] static optional<azimuth_t> parse_angle_from_name(const string& filename);
    };
}

#endif //RCSOP_DATA_INPUT_IMAGE_H
