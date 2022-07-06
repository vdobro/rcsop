#ifndef RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
#define RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H

#include "utils/types.h"

#include "observer.h"
#include "texture.h"

class AzimuthMinimapProvider {
private:
    map<height_t, map<azimuth_t, Texture>> _data;
public:
    explicit AzimuthMinimapProvider(const path& input_root_path);

    [[nodiscard]] const Texture& for_position(const Observer& observer) const;
};

#endif //RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
