#include "observer.h"
#include "observed_point.h"
#include "utils/mapping.h"

#include <execution>
#include <utility>
#include <ranges>

Observer::Observer(optional<ObserverPosition> camera_position,
                   path filepath,
                   camera source_camera,
                   double units_per_centimeter,
                   CameraCorrectionParams camera_correction)
        : _position(camera_position),
          _source_filepath(std::move(filepath)),
          _camera(std::move(source_camera)),
          _units_per_centimeter(units_per_centimeter) {
    const double pitch_correction_radians = (camera_correction.pitch * M_PI) / 180.;
    this->_correction_transform = Eigen::AngleAxis<double>(pitch_correction_radians, Vector3d::UnitX());
}

using plane = Eigen::Hyperplane<double, 3>;

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

static inline double arc_sin(double x) {
    return asin(x) * 180. / M_PI;
}

static inline observed_point observe_point(
        const ScoredPoint& point,
        const Vector3d& observer_position,
        const Vector3d& direction_right,
        const Vector3d& direction_up,
        const plane& vertical_plane,
        const plane& horizontal_plane,
        const double& units_per_centimeter) {

    const Vector3d point_position = point.position();

    auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
    auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

    auto distance_to_camera = (point_position - observer_position).norm();

    auto is_to_the_right = (point_position - observer_position).dot(direction_right) > 0;
    auto is_up_above = (point_position - observer_position).dot(direction_up) > 0;

    auto horizontal_angle = (is_to_the_right ? 1 : -1) * arc_sin(distance_to_vertical_plane / distance_to_camera);
    auto vertical_angle = (is_up_above ? 1 : -1) * arc_sin(distance_to_horizontal_plane / distance_to_camera);

    observed_point point_info = {
            .position = point.position(),
            .id = point.id(),
            .distance_in_world = distance_to_camera / units_per_centimeter,
            .vertical_angle = vertical_angle,
            .horizontal_angle = horizontal_angle,
    };
    return point_info;
}

shared_ptr<vector<observed_point>> Observer::observe_points(
        const vector<ScoredPoint>& camera_points) const {

    const auto right = get_right();
    const auto up = get_up();
    const auto observer_position = this->_camera.position();

    const plane vertical_plane(right, observer_position);
    const plane horizontal_plane(up, observer_position);
    const double units_per_cm = this->_units_per_centimeter;

    auto result = map_vec_shared<ScoredPoint, observed_point, true>(
            camera_points,
            [&observer_position, &right, &up, &vertical_plane, &horizontal_plane, &units_per_cm](
                    const auto& point) {
                return observe_point(point,
                        observer_position,
                        right, up,
                        vertical_plane, horizontal_plane,
                        units_per_cm
                );
            });

    return (result);
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
