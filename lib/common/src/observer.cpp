#include "observer.h"
#include "observed_point.h"

#include <utility>

Observer::Observer(const ObserverPosition& camera_position,
                   camera  source_camera,
                   path filepath,
                   double world_scale)
        : _position(camera_position),
          _source_filepath(std::move(filepath)),
          _camera(std::move(source_camera)),
          _worldScale(world_scale) {}

typedef Eigen::Hyperplane<double, 3> plane;

Vector3d Observer::get_right() const {
    Vector3d camera_right;
    camera_right.setZero();
    camera_right.x() = 1;
    Vector3d direction = _camera.transform_to_world(camera_right) - _camera.position();
    return direction.normalized();
}

const static int CAMERA_HEIGHT_CORRECTION_DEGREES = -5;
const static double CAMERA_CORRECTION_RADIANS = (CAMERA_HEIGHT_CORRECTION_DEGREES * M_PI) / 180;
const Eigen::Transform<double, 3, Eigen::Affine> CORRECTION_TRANSFORM(
        Eigen::AngleAxis<double>(CAMERA_CORRECTION_RADIANS, Vector3d::UnitX()));


Vector3d Observer::get_up() const {
    Vector3d camera_up;
    camera_up.setZero();
    camera_up.y() = -1;
    Vector3d direction = _camera.transform_to_world(camera_up.transpose() * CORRECTION_TRANSFORM.rotation()) - _camera.position();
    return direction.normalized();
}

ObserverPosition Observer::position() const {
    return this->_position;
}

#define ARC_SIN(X) (asin(X) * 180.f / M_PI)

static inline observed_point observe_point(
        point_id_t id,
        const ScoredPoint& point,
        const Vector3d& observer_position,
        const Vector3d& direction_right,
        const Vector3d& direction_up,
        const plane& vertical_plane,
        const plane& horizontal_plane,
        double world_scale) {

    Vector3d point_position = point.position();

    auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
    auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

    auto distance_to_camera = (point_position - observer_position).norm();

    auto is_to_the_right = (point_position - observer_position).dot(direction_right) > 0;
    auto is_up_above = (point_position - observer_position).dot(direction_up) > 0;

    auto horizontal_angle = (is_to_the_right ? 1 : -1) * ARC_SIN(distance_to_vertical_plane / distance_to_camera);
    auto vertical_angle = (is_up_above ? 1 : -1) * ARC_SIN(distance_to_horizontal_plane / distance_to_camera);

    observed_point point_info = {
            .position = point.position(),
            .id = id,
            .distance_in_world = distance_to_camera / world_scale,
            .vertical_angle = vertical_angle,
            .horizontal_angle = horizontal_angle,
    };
    return point_info;
}

vector<observed_point> Observer::observe_points(const ScoredPointMap& camera_points) const {
    const auto height = this->position().height;
    const auto height_offset = static_cast<double>(height - 40) * _worldScale; //TODO default height provider

    const auto right = get_right();
    const auto up = get_up();
    const Vector3d observer_position = this->_camera.position() + (height_offset * up);

    const plane vertical_plane = plane(right, observer_position);
    const plane horizontal_plane = plane(up, observer_position);

    vector<observed_point> result;
    for (const auto& point_pair: camera_points) {
        auto id = point_pair.first;
        auto point = point_pair.second;

        observed_point observed_p = observe_point(
                id, point, observer_position,
                right, up,
                vertical_plane, horizontal_plane,
                this->_worldScale);
        result.push_back(observed_p);
    }
    return result;

}

path Observer::source_image_path() const {
    return this->_source_filepath;
}

camera Observer::native_camera() const {
    return this->_camera;
}
