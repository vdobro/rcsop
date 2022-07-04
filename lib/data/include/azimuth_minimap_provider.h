#ifndef RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
#define RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H

#include "utils/types.h"

#include "observer.h"
#include "texture.h"

class AzimuthMinimapProvider {
private:
    shared_ptr<map<height_t, map<azimuth_t, shared_ptr<Texture>>>> _data;
public:
    explicit AzimuthMinimapProvider(const path& input_root_path);

    [[nodiscard]] shared_ptr<Texture> for_position(const Observer& observer) const;
};

#endif //RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
