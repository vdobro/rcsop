#ifndef RCSOP_COMMON_OBSERVER_POSITION_H
#define RCSOP_COMMON_OBSERVER_POSITION_H

#include "utils/types.h"

namespace rcsop::common {
    using height_t = long;
    using azimuth_t = long;

    struct ObserverPosition {
        height_t height;
        azimuth_t azimuth;

        [[nodiscard]] string str() const {
            return "height = " + std::to_string(height) + ", " +
                   "angle = " + std::to_string(azimuth);
        }
    };
}

#endif //RCSOP_COMMON_OBSERVER_POSITION_H
