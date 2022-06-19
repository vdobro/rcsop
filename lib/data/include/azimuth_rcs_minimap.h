#ifndef RCSOP_DATA_AZIMUTH_RCS_MINIMAP_H
#define RCSOP_DATA_AZIMUTH_RCS_MINIMAP_H

#include "utils/types.h"
#include "observer_position.h"

class AzimuthRcsMinimap {
private:
    path _file_path;
    ObserverPosition _camera_position;
public:
    explicit AzimuthRcsMinimap(path file_path,
                               const ObserverPosition& position);

    [[nodiscard]] ObserverPosition position() const;

    [[nodiscard]] path file_path() const;
};

#endif //RCSOP_DATA_AZIMUTH_RCS_MINIMAP_H
