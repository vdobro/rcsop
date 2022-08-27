#ifndef RCSOP_COMMON_OBSERVER_POSITION_H
#define RCSOP_COMMON_OBSERVER_POSITION_H

#include "utils/types.h"

namespace rcsop::common {
    using height_t = long;
    using azimuth_t = long;

    struct ObserverPosition {
        height_t height{};
        azimuth_t azimuth{};

        [[nodiscard]] auto str() const -> string;

        bool operator<(const ObserverPosition& other) const;

        [[nodiscard]] auto distance_to(const ObserverPosition& other,
                                       double world_distance_to_origin) const -> double;
    };
}

#endif //RCSOP_COMMON_OBSERVER_POSITION_H
