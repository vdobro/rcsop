#ifndef RCSOP_COMMON_IDENTIFIED_POINT_H
#define RCSOP_COMMON_IDENTIFIED_POINT_H

#include <utility>

#include "utils/types.h"
#include "utils/points.h"
#include "id_point.h"

namespace rcsop::common {
    using rcsop::common::utils::points::point_id_t;
    using rcsop::common::utils::points::vec3;

    class SimplePoint : public IdPoint {
    private:
        point_id_t _point_id = -1;
        vec3 _position = vec3::Zero();

    public:
        SimplePoint() = default;

        SimplePoint(point_id_t id, vec3 position);

        [[nodiscard]] vec3 position() const override;

        [[nodiscard]] point_id_t id() const override;

    };

}

#endif //RCSOP_COMMON_IDENTIFIED_POINT_H
