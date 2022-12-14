#ifndef RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
#define RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H

#include "utils/types.h"

#include "observer.h"
#include "texture.h"

namespace rcsop::data {
    using rcsop::common::height_t;
    using rcsop::common::azimuth_t;
    using rcsop::common::Texture;
    using rcsop::common::Observer;

    class AzimuthMinimapProvider {
    private:
        map<height_t, map<azimuth_t, Texture>> _data;
    public:
        explicit AzimuthMinimapProvider(const path& input_root_path);

        [[nodiscard]] auto for_position(const Observer& observer) const -> const Texture&;

        [[nodiscard]] static bool is_available_at(const path& root_path);
    };
}

#endif //RCSOP_DATA_AZIMUTH_MINIMAP_PROVIDER_H
