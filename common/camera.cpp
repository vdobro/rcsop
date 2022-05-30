#include "camera.h"

#include "utils/types.h"
#include "utils/colmap.h"

Vector3d camera::transform_to_world(const Vector3d& image_xyz) const {
    return model_image.InverseProjectionMatrix() * image_xyz.homogeneous();
}

Vector3d camera::get_position() const {
    Vector3d origin = Vector3d();
    origin.setZero();
    return transform_to_world(origin);
}

camera::camera(const Image& image, const Reconstruction& model) {
    this->model_image = image;
    this->model_camera = model.Camera(image.CameraId());
}

Eigen::Vector3d camera::get_direction() const {
    return model_image.ViewingDirection();
}

camera_id_t camera::id() const {
    return model_camera.CameraId();
}

Vector2d camera::project_from_image(const Vector2d& point) const {
    return model_camera.WorldToImage(point);
}

size_t camera::width() const {
    return model_camera.Width();
}

size_t camera::height() const {
    return model_camera.Height();
}

string camera::get_name() const {
    return model_image.Name();
}

vector<image_point> camera::project_to_image(const vector<scored_point>& points) const {
    std::vector<image_point> result;
    result.resize(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        const scored_point& point = points[i];
        Vector2d position_normalized = (model_image.ProjectionMatrix() * point.position().homogeneous()).hnormalized();
        double distance_to_camera = (get_position() - point.position()).norm();
        image_point projected_point = image_point(position_normalized, distance_to_camera, point.score_to_dB());
        result[i] = projected_point;
    }
    return result;
}
