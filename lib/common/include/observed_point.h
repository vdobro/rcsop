#ifndef RCSOP_COMMON_OBSERVED_POINT_H
#define RCSOP_COMMON_OBSERVED_POINT_H

#include "utils/types.h"
#include "utils/points.h"

namespace rcsop::common {
    using rcsop::common::utils::points::vec3;
    using rcsop::common::utils::points::point_id_t;

    struct observed_point {
        vec3 position = vec3::Zero();
        point_id_t id = 0;
        double distance_in_world = 0;
        double vertical_angle = 0;
        double horizontal_angle = 0;
    };
}

#endif //RCSOP_COMMON_OBSERVED_POINT_H
