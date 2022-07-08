#ifndef RCSOP_DATA_INPUT_IMAGE_H
#define RCSOP_DATA_INPUT_IMAGE_H

#include <optional>
using std::optional;

#include "utils/types.h"
#include "observer_position.h"

class CameraInputImage {
private:
    path _file_path;
    optional<ObserverPosition> _camera_position;

public:
    explicit CameraInputImage(const path& path);

    [[nodiscard]] optional<ObserverPosition> position() const;

    [[nodiscard]] path file_path() const;

    static optional<ObserverPosition> parse_position_from_file_path(const path& path);

    static optional<ObserverPosition> parse_position_from_name(const string& filename);

    static optional<azimuth_t> parse_angle_from_name(const string& filename);
};

#endif //RCSOP_DATA_INPUT_IMAGE_H
