#ifndef RCSOP_COMMON_CAMERA_H
#define RCSOP_COMMON_CAMERA_H

#include "colmap/util/types.h"
#include "colmap/base/reconstruction.h"

#include "utils/points.h"

#include "image_point.h"
#include "scored_point.h"

namespace rcsop::common {
    using camera_id_t = size_t;

    using rcsop::common::utils::points::Vector2d;
    using rcsop::common::utils::points::Vector3d;

    class camera {
    private:
        colmap::Camera model_camera;
        colmap::Image model_image;

    public:
        camera(const colmap::Image& image,
               const colmap::Reconstruction& model);

        [[nodiscard]] camera_id_t id() const;

        /**
         * Converts point from camera coordinate system to world coordinates
         * @param local_coordinates xyz in the local coordinate system of this camera
         */
        [[nodiscard]] Vector3d transform_to_world(const Vector3d& local_coordinates) const;

        [[nodiscard]] Vector3d position() const;

        [[nodiscard]] Vector3d direction() const;

        [[nodiscard]] Vector2d project_from_image(const Vector2d& point) const;

        [[nodiscard]] size_t image_width() const;

        [[nodiscard]] size_t image_height() const;

        [[nodiscard]] string get_name() const;

        [[nodiscard]] vector<ImagePoint> project_to_image(const vector<ScoredPoint>& points) const;
    };
}

#endif //RCSOP_COMMON_CAMERA_H
