#ifndef RCSOP_COMMON_COLMAP_H
#define RCSOP_COMMON_COLMAP_H

#include "colmap/base/reconstruction.h"
#include "colmap/util/types.h"

namespace rcsop::common::utils::sparse {
    using colmap::Reconstruction;
    using colmap::Image;
    using colmap::Point3D;
    using colmap::Camera;

    using color_vec = Eigen::Vector4ub;
}

#endif //RCSOP_COMMON_COLMAP_H
