#ifndef RCSOP_DATA_INPUT_IMAGE_H
#define RCSOP_DATA_INPUT_IMAGE_H

#include "utils/types.h"
#include "observer_position.h"

namespace rcsop::data {
    using rcsop::common::ObserverPosition;
    using rcsop::common::azimuth_t;

    class CameraInputImage {
    private:
        path _file_path;
        optional<ObserverPosition> _camera_position;

    public:
        explicit CameraInputImage(const path& path);

        [[nodiscard]] optional<ObserverPosition> position() const;

        [[nodiscard]] path file_path() const;

        [[nodiscard]] static optional<ObserverPosition> parse_position_from_file_path(const path& path);

        [[nodiscard]] static optional<ObserverPosition> parse_position_from_name(const string& filename);

        [[nodiscard]] static optional<azimuth_t> parse_angle_from_name(const string& filename);
    };
}

#endif //RCSOP_DATA_INPUT_IMAGE_H
