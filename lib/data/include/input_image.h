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

    static optional<ObserverPosition> parse_filename(const path& path);
};

#endif //RCSOP_DATA_INPUT_IMAGE_H
