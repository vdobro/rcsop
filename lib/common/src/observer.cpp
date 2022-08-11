#include "observer.h"

#include <ranges>
#include <utility>

#include "utils/mapping.h"

#include "observed_point.h"

namespace rcsop::common {
    using rcsop::common::utils::map_vec_shared;

    Observer::Observer(optional<ObserverPosition> camera_position,
                       path filepath,
                       shared_ptr<ObserverCamera const> observer_camera)
            : _position(camera_position),
              _source_filepath(std::move(filepath)),
              _camera(std::move(observer_camera)) {}

    ObserverPosition Observer::position() const {
        if (!this->has_position()) {
            throw std::logic_error("Observer for " + this->source_image_path().filename().string()
                                   + " does not have a predefined position");
        }
        return *this->_position;
    }

    vec3_spherical Observer::cartesian_to_spherical(const vec3& point) {
        const double distance = point.norm();
        const double
                x = point.x(),
                y = point.y(),
                z = point.z();

        double phi;
        double theta;

        if (x > 0) {
            phi = atan(y / x);
        } else if (x == 0) {
            if (y >= 0) {
                phi = M_PI_2; // y = 0 undefined by definition, but pi/2 is okay as a fallback here
            } else { // y < 0
                phi = -M_PI_2;
            }
        } else { // x < 0
            if (y >= 0) {
                phi = atan(y / x) + M_PI;
            } else { // y < 0
                phi = atan(y / x) - M_PI;
            }
        }
        theta = (distance != 0) ? acos(z / distance) : M_PI_2;

        return {
                .radial = distance,
                .azimuthal = phi * 180. / M_PI,
                .polar = theta * 180. / M_PI,
        };
    }

    vec3 Observer::spherical_to_cartesian(const vec3_spherical& point) {
        const auto& [radial, azimuthal, polar] = point;
        const auto phi = azimuthal * M_PI / 180.;
        const auto theta = polar * M_PI / 180.;

        const auto x = radial * cos(phi) * sin(theta);
        const auto y = radial * sin(phi) * sin(theta);
        const auto z = radial * cos(theta);
        return {x, y, z};
    }

    observed_point Observer::observe_point(const ScoredPoint& point) const {
        const auto world_point = point.position();
        const auto distance = _camera->distance_to_camera(world_point);
        const auto local_point = _camera->map_to_observer_local(world_point);

        const auto& [_, azimuthal, polar] = cartesian_to_spherical(local_point);

        double horizontal_angle, vertical_angle;
        if (distance == 0.) {
            horizontal_angle = 0;
            vertical_angle = 0;
        } else {
            horizontal_angle = 90 - abs(azimuthal);
            vertical_angle = 90 - polar;
        }
        observed_point point_info = {
                .position = point.position(),
                .id = point.id(),
                .distance_in_world = distance / this->_units_per_centimeter,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
        return point_info;
    }

    vec3 Observer::project_position(const observed_point& observed_point) const {
        const auto& [position, _, distance_in_world, vertical_angle, horizontal_angle] = observed_point;
        if (distance_in_world == 0) {
            return this->_camera->native_camera().position();
        }
        auto radial = distance_in_world * this->_units_per_centimeter;
        auto azimuthal = 90 - horizontal_angle;
        auto polar = 90 - vertical_angle;

        const auto cartesian_local = spherical_to_cartesian({.radial = radial, .azimuthal = azimuthal, .polar = polar});
        return _camera->map_to_world(cartesian_local);
    }

    shared_ptr<vector<observed_point>> Observer::observe_points(
            const vector<ScoredPoint>& camera_points) const {
        auto result = map_vec_shared<ScoredPoint, observed_point, true>(
                camera_points,
                [this](const auto& point) {
                    return observe_point(point);
                });

        return result;
    }

    shared_ptr<vector<vec3>> Observer::project_observed_positions(
            const vector<observed_point>& positions) const {
        auto result = map_vec_shared<observed_point, vec3, true>(
                positions, [this](const auto& position) {
                    return project_position(position);
                });
        return result;
    }

    path Observer::source_image_path() const {
        return this->_source_filepath;
    }

    camera Observer::native_camera() const {
        return this->_camera->native_camera();
    }

    bool Observer::has_position() const {
        return this->_position.has_value();
    }

    void Observer::set_units_per_centimeter(double units) {
        this->_units_per_centimeter = units;
    }
}
