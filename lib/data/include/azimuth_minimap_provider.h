#ifndef RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
#define RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H

#include "utils/types.h"
#include "observer_position.h"

#include "azimuth_rcs_minimap.h"

class AzimuthMinimapProvider {
private:
    shared_ptr<map<height_t, map<azimuth_t, shared_ptr<AzimuthRcsMinimap>>>> _data;
public:
    explicit AzimuthMinimapProvider(const path& input_root_path);

    [[nodiscard]] shared_ptr<AzimuthRcsMinimap> at_position(const ObserverPosition& position) const;
};

#endif //RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
