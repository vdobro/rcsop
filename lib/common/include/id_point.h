//
// Created by vd on 22.08.22.
//

#ifndef RCSOP_COMMON_IDENTIFIED_POINT_H
#define RCSOP_COMMON_IDENTIFIED_POINT_H

#include <utility>

#include "utils/types.h"
#include "utils/points.h"

namespace rcsop::common {
    using rcsop::common::utils::points::point_id_t;
    using rcsop::common::utils::points::vec3;

    class IdPoint {
    private:
        point_id_t _point_id = -1;
        vec3 _position = vec3::Zero();

    public:
        IdPoint() = default;

        IdPoint(point_id_t id, vec3 position);

        [[nodiscard]] vec3 position() const;

        [[nodiscard]] point_id_t id() const;

    };

}

#endif //RCSOP_COMMON_IDENTIFIED_POINT_H
