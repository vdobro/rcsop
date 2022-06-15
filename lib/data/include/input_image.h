#ifndef RCSO_DATA_INPUT_IMAGE_H
#define RCSO_DATA_INPUT_IMAGE_H

#include <filesystem>
using std::filesystem::path;

#include "az_data.h"
#include "azimuth_rcs_map.h"

class InputImage {
private:
    ObserverPosition _camera_position;
    path _file_path;

public:
    explicit InputImage(const path& path);
    [[nodiscard]] ObserverPosition position() const;
    [[nodiscard]] azimuth_t azimuth() const;
    [[nodiscard]] path file_path() const;
};

#endif //RCSO_DATA_INPUT_IMAGE_H
