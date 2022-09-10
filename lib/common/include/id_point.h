#ifndef RCSOP_COMMON_ID_POINT_H
#define RCSOP_COMMON_ID_POINT_H

#include "utils/types.h"
#include "utils/points.h"

namespace rcsop::common {
    using rcsop::common::utils::points::point_id_t;
    using rcsop::common::utils::points::vec3;

    class IdPoint {
    public:
        virtual ~IdPoint() = default;

        [[nodiscard]] virtual vec3 position() const = 0;

        [[nodiscard]] virtual point_id_t id() const = 0;
    };
}

#endif //RCSOP_COMMON_ID_POINT_H
