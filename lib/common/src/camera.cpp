#include "camera.h"

#include "utils/sparse.h"
#include "utils/mapping.h"

namespace rcsop::common {

    using rcsop::common::utils::sparse::Image;
    using rcsop::common::utils::sparse::Reconstruction;
    using rcsop::common::utils::points::Vector2d;
    using rcsop::common::utils::points::Vector3d;

    camera::camera(const Image& image, const Reconstruction& model) {
        this->model_image = image;
        this->model_camera = model.Camera(image.CameraId());
    }

    Vector3d camera::transform_to_world(const Vector3d& local_coordinates) const {
        return model_image.InverseProjectionMatrix() * local_coordinates.homogeneous();
    }

    Vector3d camera::position() const {
        Vector3d origin = Vector3d();
        origin.setZero();
        return transform_to_world(origin);
    }

    Vector3d camera::direction() const {
        return model_image.ViewingDirection();
    }

    camera_id_t camera::id() const {
        return model_camera.CameraId();
    }

    Vector2d camera::project_from_image(const Vector2d& point) const {
        return model_camera.WorldToImage(point);
    }

    size_t camera::image_width() const {
        return model_camera.Width();
    }

    size_t camera::image_height() const {
        return model_camera.Height();
    }

    string camera::get_name() const {
        return model_image.Name();
    }

    vector<ImagePoint> camera::project_to_image(const vector<ScoredPoint>& points) const {
        const auto camera_position = this->position();
        const auto image_projection_matrix = model_image.ProjectionMatrix();

        return utils::map_vec<ScoredPoint, ImagePoint>(points, [camera_position, image_projection_matrix]
                (const ScoredPoint& point) -> ImagePoint {
            Vector2d position_normalized = (image_projection_matrix * point.position().homogeneous()).hnormalized();
            double distance_to_camera = (camera_position - point.position()).norm();
            ImagePoint projected_point = ImagePoint(position_normalized, distance_to_camera, point.score_to_dB());
            return projected_point;
        });
    }
}
