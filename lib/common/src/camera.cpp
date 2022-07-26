#include "camera.h"

#include "utils/sparse.h"
#include "utils/mapping.h"

namespace rcsop::common {
    using rcsop::common::utils::sparse::Image;
    using rcsop::common::utils::sparse::Reconstruction;
    using rcsop::common::utils::points::vec2;
    using rcsop::common::utils::points::vec3;

    camera::camera(const Image& image,
                   const Reconstruction& model) : _model_image(image) {
        this->_model_camera = model.Camera(image.CameraId());
    }

    vec3 camera::transform_to_world(const vec3& local_coordinates) const {
        return _model_image.InverseProjectionMatrix() * local_coordinates.homogeneous();
    }

    vec3 camera::transform_to_local(const vec3& world_coordinates) const {
        return _model_image.ProjectionMatrix() * world_coordinates.homogeneous();
    }

    vec3 camera::position() const {
        return transform_to_world(vec3::Zero());
    }

    camera_id_t camera::id() const {
        return _model_camera.CameraId();
    }

    vec2 camera::project_from_image(const vec2& point) const {
        return _model_camera.WorldToImage(point);
    }

    size_t camera::image_width() const {
        return _model_camera.Width();
    }

    size_t camera::image_height() const {
        return _model_camera.Height();
    }

    string camera::get_name() const {
        return _model_image.Name();
    }

    vector<ImagePoint> camera::project_to_image(const vector<ScoredPoint>& points) const {
        const auto camera_position = this->position();
        const auto image_projection_matrix = _model_image.ProjectionMatrix();

        return utils::map_vec<ScoredPoint, ImagePoint>(points, [camera_position, image_projection_matrix]
                (const ScoredPoint& point) -> ImagePoint {
            vec2 position_normalized = (image_projection_matrix * point.position().homogeneous()).hnormalized();
            double distance_to_camera = (camera_position - point.position()).norm();
            ImagePoint projected_point = ImagePoint(position_normalized, distance_to_camera, point.score_to_dB());
            return projected_point;
        });
    }
}
