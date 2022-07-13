#ifndef RCSOP_COMMON_POINTS_H
#define RCSOP_COMMON_POINTS_H

#include <Eigen/Core>

#include "utils/types.h"

namespace rcsop::common::utils::points {
    using vec2 = Eigen::Vector2d;
    using vec3 = Eigen::Vector3d;

    using point_id_t = ulong;
    using point_pair = pair<point_id_t, vec3>;
}

#endif //RCSOP_COMMON_POINTS_H
