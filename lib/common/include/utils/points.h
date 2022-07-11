#ifndef RCSOP_COMMON_POINTS_H
#define RCSOP_COMMON_POINTS_H

#include <Eigen/Core>

#include "utils/types.h"

namespace rcsop::common::utils::points {
    using Eigen::Vector2d;
    using Eigen::Vector3d;

    using point_id_t = ulong;
    using point_pair = pair<point_id_t, Vector3d>;
}

#endif //RCSOP_COMMON_POINTS_H
