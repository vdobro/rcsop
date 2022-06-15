#ifndef RCSOP_DATA_INPUT_IMAGE_H
#define RCSOP_DATA_INPUT_IMAGE_H

#include <filesystem>
using std::filesystem::path;

#include "az_data.h"
#include "azimuth_rcs_map.h"

class InputImage {
private:
    path _file_path;
    ObserverPosition _camera_position;

public:
    explicit InputImage(const path& path);
    [[nodiscard]] ObserverPosition position() const;

    [[nodiscard]] path file_path() const;
};

#endif //RCSOP_DATA_INPUT_IMAGE_H
