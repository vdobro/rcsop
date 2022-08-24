#ifndef RCSOP_COMMON_CAMERA_H
#define RCSOP_COMMON_CAMERA_H

#include "colmap/util/types.h"
#include "colmap/base/reconstruction.h"

#include "utils/points.h"

#include "image_point.h"
#include "scored_point.h"

namespace rcsop::common {
    using camera_id_t = colmap::image_t;

    using rcsop::common::utils::points::vec2;
    using rcsop::common::utils::points::vec3;

    class ModelCamera {
    private:
        colmap::Camera _model_camera;
        colmap::Image _model_image;

    public:
        ModelCamera(const colmap::Image& image,
                    const colmap::Reconstruction& model);

        [[nodiscard]] camera_id_t id() const;

        /**
         * Converts point from camera coordinate system to world coordinates
         * @param local_coordinates xyz in the local coordinate system of this camera
         */
        [[nodiscard]] vec3 transform_to_world(const vec3& local_coordinates) const;

        [[nodiscard]] vec3 transform_to_local(const vec3& world_coordinates) const;

        [[nodiscard]] vec3 position() const;

        [[nodiscard]] vec2 project_from_image(const vec2& point) const;

        [[nodiscard]] size_t image_width() const;

        [[nodiscard]] size_t image_height() const;

        [[nodiscard]] string get_name() const;

        [[nodiscard]] vector<ImagePoint> project_to_image(const vector<ScoredPoint>& points) const;
    };
}

#endif //RCSOP_COMMON_CAMERA_H
