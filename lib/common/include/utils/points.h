#ifndef RCSOP_COMMON_POINTS_H
#define RCSOP_COMMON_POINTS_H

#include <Eigen/Core>

#include "utils/types.h"

namespace rcsop::common::utils::points {
    using vec2 = Eigen::Vector2d;
    using vec3 = Eigen::Vector3d;

    struct vec3_spherical {
        double radial{};
        double azimuthal{};
        double polar{};
    };

    using point_id_t = ulong;
    using point_pair = pair<point_id_t, vec3>;

    auto find_farthest(const vec3& reference_point,
                       const vector<vec3>& search_values) -> vec3;
}

#endif //RCSOP_COMMON_POINTS_H
