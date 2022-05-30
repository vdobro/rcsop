#ifndef SFM_COLORING_COMMON_CAMERA_H
#define SFM_COLORING_COMMON_CAMERA_H

#include "colmap/util/types.h"
#include "colmap/base/reconstruction.h"

#include "utils/vector.h"

#include "image_point.h"
#include "scored_point.h"

typedef size_t camera_id_t;

class camera {
private:
    colmap::Camera model_camera;
    colmap::Image model_image;

public:
    camera(const colmap::Image& image,
           const colmap::Reconstruction& model);

    [[nodiscard]] camera_id_t id() const;

    [[nodiscard]] Eigen::Vector3d transform_to_world(const Eigen::Vector3d& image_xyz) const;

    [[nodiscard]] Eigen::Vector3d get_position() const;

    [[nodiscard]] Eigen::Vector3d get_direction() const;

    [[nodiscard]] Vector2d project_from_image(const Vector2d& point) const;

    [[nodiscard]] size_t width() const;

    [[nodiscard]] size_t height() const;

    [[nodiscard]] string get_name() const;

    [[nodiscard]] vector<image_point> project_to_image(const vector<scored_point>& points) const;
};

#endif //SFM_COLORING_COMMON_CAMERA_H
