#include "azimuth_rcs_minimap.h"

#include <utility>

AzimuthRcsMinimap::AzimuthRcsMinimap(path file_path,
                                     const ObserverPosition& position)
        : _file_path(std::move(file_path)),
          _camera_position(position) {}

ObserverPosition AzimuthRcsMinimap::position() const {
    return this->_camera_position;
}

path AzimuthRcsMinimap::file_path() const {
    return this->_file_path;
}
