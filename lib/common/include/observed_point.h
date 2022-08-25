#ifndef RCSOP_COMMON_OBSERVED_POINT_H
#define RCSOP_COMMON_OBSERVED_POINT_H

#include "utils/types.h"
#include "utils/points.h"

namespace rcsop::common {
    using rcsop::common::utils::points::vec3;
    using rcsop::common::utils::points::point_id_t;

    struct observed_point {
        vec3 position = vec3::Zero();
        point_id_t id{};
        double distance_in_world{};
        double vertical_angle{};
        double horizontal_angle{};
    };

    using observed_factor_func = function<double(const observed_point&)>;
}

#endif //RCSOP_COMMON_OBSERVED_POINT_H
