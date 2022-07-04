#include "observer.h"
#include "observed_point.h"
#include "utils/mapping.h"

#include <execution>
#include <utility>
#include <ranges>

Observer::Observer(optional<ObserverPosition> camera_position,
                   path filepath,
                   camera source_camera,
                   double world_scale,
                   CameraCorrectionParams camera_correction)
        : _position(camera_position),
          _source_filepath(std::move(filepath)),
          _camera(std::move(source_camera)),
          _world_scale(world_scale) {
    if (this->has_position()) {
        const auto height = this->position().height;
        this->_height_offset = static_cast<double>(height - camera_correction.default_height) * _world_scale;
    }
    const double pitch_correction_radians = (camera_correction.pitch * M_PI) / 180;
    this->_correction_transform = Eigen::AngleAxis<double>(pitch_correction_radians, Vector3d::UnitX());
}

typedef Eigen::Hyperplane<double, 3> plane;

Vector3d Observer::get_right() const {
    Vector3d camera_right;
    camera_right.setZero();
    camera_right.x() = 1;
    Vector3d direction = _camera.transform_to_world(camera_right) - _camera.position();
    return direction.normalized();
}

Vector3d Observer::get_up() const {
    Vector3d camera_up;
    camera_up.setZero();
    camera_up.y() = -1;
    Vector3d direction =
            _camera.transform_to_world(camera_up.transpose() * this->_correction_transform.rotation()) -
            _camera.position();
    return direction.normalized();
}

ObserverPosition Observer::position() const {
    if (!this->has_position()) {
        throw std::logic_error("Observer for " + this->source_image_path().filename().string()
                               + " does not have a predefined position");
    }
    return *this->_position;
}

#define ARC_SIN(X) (asin(X) * 180.f / M_PI)

static inline observed_point observe_point(
        const point_id_t& id,
        const ScoredPoint& point,
        const Vector3d& observer_position,
        const Vector3d& direction_right,
        const Vector3d& direction_up,
        const plane& vertical_plane,
        const plane& horizontal_plane,
        const double& world_scale) {

    const Vector3d point_position = point.position();

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

shared_ptr<vector<observed_point>> Observer::observe_points(
        const vector<ScoredPoint>& camera_points) const {

    const auto right = get_right();
    const auto up = get_up();
    const Vector3d observer_position = this->_camera.position() + (this->_height_offset * up);

    const plane vertical_plane = plane(right, observer_position);
    const plane horizontal_plane = plane(up, observer_position);
    const double world_scale = this->_world_scale;

    auto result = map_vec<ScoredPoint, observed_point, true>(
            camera_points,
            [&observer_position, &right, &up, &vertical_plane, &horizontal_plane, &world_scale](
                    const auto& point) {
                return observe_point(
                        point.id(), point,
                        observer_position,
                        right, up,
                        vertical_plane, horizontal_plane,
                        world_scale
                );
            });

    return make_shared<vector<observed_point>>(result);
}

path Observer::source_image_path() const {
    return this->_source_filepath;
}

camera Observer::native_camera() const {
    return this->_camera;
}

bool Observer::has_position() const {
    return this->_position.has_value();
}
