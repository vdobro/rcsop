#include "observer.h"

#include <utility>
#include <ranges>

#include "utils/mapping.h"

#include "observed_point.h"

namespace rcsop::common {
    using rcsop::common::utils::map_vec_shared;

    Observer::Observer(optional<ObserverPosition> camera_position,
                       path filepath,
                       camera source_camera,
                       camera_options options)
            : _position(camera_position),
              _source_filepath(std::move(filepath)),
              _camera(std::move(source_camera)) {
        const double pitch_correction_radians = (options.pitch_correction * M_PI) / 180.;
        this->_correction_transform = Eigen::AngleAxis<double>(pitch_correction_radians, vec3::UnitX());
    }

    vec3 Observer::get_right() const {
        vec3 camera_right;
        camera_right.setZero();
        camera_right.x() = 1;
        vec3 direction = _camera.transform_to_world(camera_right) - _camera.position();
        return direction.normalized();
    }

    vec3 Observer::get_up() const {
        vec3 camera_up;
        camera_up.setZero();
        camera_up.y() = -1; // the Y axis points to the bottom in COLMAP, see https://colmap.github.io/format.html#images-txt
        vec3 direction =
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

    observed_point Observer::observe_point(const ScoredPoint& point,
                                           const observer_position_props& props) const {

        const vec3 point_position = point.position();

        auto distance_to_vertical_plane = props.vertical_plane.absDistance(point_position);
        auto distance_to_horizontal_plane = props.horizontal_plane.absDistance(point_position);

        auto distance_to_camera = (point_position - props.observer_position).norm();

        auto is_to_the_right = (point_position - props.observer_position).dot(props.direction_right) > 0;
        auto is_up_above = (point_position - props.observer_position).dot(props.direction_up) > 0;

        auto horizontal_angle = (is_to_the_right ? 1 : -1) * arc_sin(distance_to_vertical_plane / distance_to_camera);
        auto vertical_angle = (is_up_above ? 1 : -1) * arc_sin(distance_to_horizontal_plane / distance_to_camera);

        observed_point point_info = {
                .position = point.position(),
                .id = point.id(),
                .distance_in_world = distance_to_camera / this->_units_per_centimeter,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
        return point_info;
    }

    observer_position_props Observer::get_observer_props() const {
        auto right = get_right();
        auto up = get_up();
        auto observer_position = this->_camera.position();

        plane vertical_plane(right, observer_position);
        plane horizontal_plane(up, observer_position);

        observer_position_props props = {
                .observer_position = observer_position,
                .direction_right = right,
                .direction_up = up,
                .vertical_plane = vertical_plane,
                .horizontal_plane = horizontal_plane,
        };
        return props;
    }

    shared_ptr<vector<observed_point>> Observer::observe_points(
            const vector<ScoredPoint>& camera_points) const {
        const auto props = get_observer_props();
        auto result = map_vec_shared<ScoredPoint, observed_point, true>(
                camera_points,
                [&props, this](const auto& point) {
                    return observe_point(point, props);
                });

        return result;
    }

    vec3 Observer::project_position(const observed_point& position,
                                    const observer_position_props& observer_position_props) const {
        //TODO

        //const auto distance = position.distance_in_world * this->_units_per_centimeter;

        return this->_camera.position(); //TODO
    }

    shared_ptr<vector<vec3>> Observer::project_observed_positions(
            const vector<observed_point>& position) const {
        auto props = get_observer_props();

        auto result = map_vec_shared<observed_point, vec3, true>(
                position, [&props, this](const auto& position) {
                    return project_position(position, props);
                });
        return result;
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

    void Observer::set_units_per_centimeter(double units) {
        this->_units_per_centimeter = units;
    }
}
