#ifndef RCSOP_DATA_INPUT_IMAGE_H
#define RCSOP_DATA_INPUT_IMAGE_H

#include "utils/types.h"
#include "observer_position.h"

class CameraInputImage {
private:
    path _file_path;
    ObserverPosition _camera_position;

public:
    explicit CameraInputImage(const path& path);
    [[nodiscard]] ObserverPosition position() const;

    [[nodiscard]] path file_path() const;
};

#endif //RCSOP_DATA_INPUT_IMAGE_H
