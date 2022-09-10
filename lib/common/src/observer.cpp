#include "observer.h"

#include <ranges>
#include <utility>

#include "utils/mapping.h"

#include "observed_point.h"

namespace rcsop::common {
    using rcsop::common::utils::map_vec_shared;
    using rcsop::common::utils::filter_vec;

    const static double PI_RADIANS = 180.;
    const static double HALF_PI_RADIANS = PI_RADIANS / 2;

    Observer::Observer(optional<ObserverPosition> camera_position,
                       path filepath,
                       shared_ptr<ObserverCamera const> observer_camera,
                       data_observer_translation observer_translation,
                       observer_camera_options camera_options)
            : _position(camera_position),
              _source_filepath(std::move(filepath)),
              _camera(std::move(observer_camera)),
              _observer_translation(observer_translation),
              _camera_options(camera_options) {
        _world_roll = Eigen::AngleAxis<double>((_observer_translation.roll * M_PI) / PI_RADIANS, vec3::UnitY());
    }

    auto Observer::get_height_offset() const -> double {
        return static_cast<double>(_camera_options.height_offset) * _units_per_centimeter;
    }

    auto Observer::position() const -> ObserverPosition {
        if (!this->has_position()) {
            throw std::logic_error("Observer for " + this->source_image_path().filename().string()
                                   + " does not have a predefined position");
        }
        return *this->_position;
    }

    auto Observer::cartesian_to_spherical(const vec3& point) -> vec3_spherical {
        double distance = point.norm();
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
                .azimuthal = phi * PI_RADIANS / M_PI,
                .polar = theta * PI_RADIANS / M_PI,
        };
    }

    auto Observer::spherical_to_cartesian(const vec3_spherical& point) -> vec3 {
        const auto& [radial, azimuthal, polar] = point;
        const auto phi = azimuthal * M_PI / PI_RADIANS;
        const auto theta = polar * M_PI / PI_RADIANS;

        auto x = radial * cos(phi) * sin(theta);
        auto y = radial * sin(phi) * sin(theta);
        auto z = radial * cos(theta);
        return {x, y, z};
    }

    auto Observer::observe_point(const SimplePoint& point) const -> observed_point {
        const auto world_point = point.position();
        const auto distance = _camera->distance_to_camera(world_point);

        const auto translated_point = _camera->map_to_observer_local(world_point, get_height_offset());
        const auto local_point = undo_data_point_translation(translated_point);

        const auto& [_, azimuthal, polar] = cartesian_to_spherical(local_point);

        double horizontal_angle, vertical_angle;
        if (distance == 0.) {
            horizontal_angle = 0;
            vertical_angle = 0;
        } else {
            horizontal_angle = HALF_PI_RADIANS - abs(azimuthal);
            vertical_angle = HALF_PI_RADIANS - polar;
        }

        return {
                .position = point.position(),
                .id = point.id(),
                .distance_in_world = distance / this->_units_per_centimeter,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
    }

    auto Observer::translate_data_point(const vec3& local_point) const -> vec3 {
        return local_point.transpose() * _world_roll.rotation();
    }

    auto Observer::undo_data_point_translation(const vec3& world_point) const -> vec3 {
        return world_point.transpose() * _world_roll.rotation().transpose();
    }

    auto Observer::project_position(const observed_point& observed_point) const -> vec3 {
        const auto& [_p, _id, distance_in_world, vertical_angle, horizontal_angle] = observed_point;
        if (distance_in_world == 0) {
            return this->_camera->native_camera().position();
        }
        auto radial = distance_in_world * this->_units_per_centimeter;
        auto azimuthal = -(horizontal_angle - HALF_PI_RADIANS);
        auto polar = HALF_PI_RADIANS - vertical_angle;

        auto cartesian_local = spherical_to_cartesian({.radial = radial, .azimuthal = azimuthal, .polar = polar});
        auto translated_local = translate_data_point(cartesian_local);

        return _camera->map_to_world(translated_local, get_height_offset());
    }

    auto Observer::observe_points(const vector<SimplePoint>& camera_points) const -> shared_ptr<vector<observed_point>> {
        auto result = map_vec_shared<SimplePoint, observed_point>(
                camera_points,
                [this](const auto& point) {
                    return observe_point(point);
                });

        return result;
    }

    auto
    Observer::project_observed_positions(const vector<observed_point>& positions) const -> shared_ptr<vector<vec3>> {
        auto result = map_vec_shared<observed_point, vec3>(
                positions, [this](const auto& position) {
                    return project_position(position);
                });
        return result;
    }

    auto Observer::source_image_path() const -> path {
        return this->_source_filepath;
    }

    auto Observer::native_camera() const -> ModelCamera {
        return this->_camera->native_camera();
    }

    auto Observer::has_position() const -> bool {
        return this->_position.has_value();
    }

    void Observer::set_units_per_centimeter(double units) {
        this->_units_per_centimeter = units;
    }

    auto Observer::map_to_positions(const vector<Observer>& observers) -> set<ObserverPosition> {
        set<ObserverPosition> result;
        for (const auto& observer: observers) {
            result.insert(observer.position());
        }
        return result;
    }

    auto Observer::clone_with_camera(observer_camera_options observer_options) const -> Observer {
        auto result = Observer{
                this->_position,
                this->_source_filepath,
                this->_camera,
                this->_observer_translation,
                observer_options,
        };
        result.set_units_per_centimeter(this->_units_per_centimeter);
        return result;
    }

    auto Observer::clone_with_data(data_observer_translation data_options) const -> Observer {
        auto result = Observer{
                this->_position,
                this->_source_filepath,
                this->_camera,
                data_options,
                this->_camera_options,
        };
        result.set_units_per_centimeter(this->_units_per_centimeter);
        return result;
    }

    auto Observer::clone_with_position(ObserverPosition position) const -> Observer {
        auto result = Observer{
                position,
                this->_source_filepath,
                this->_camera,
                this->_observer_translation,
                this->_camera_options,
        };
        result.set_units_per_centimeter(this->_units_per_centimeter);
        return result;
    }

    auto Observer::clone_with_source_path(path source_image_path) const -> Observer {
        auto result = Observer {
            this->_position,
            std::move(source_image_path),
            this->_camera,
            this->_observer_translation,
            this->_camera_options,
        };
        result.set_units_per_centimeter(this->_units_per_centimeter);
        return result;

    }

    auto Observer::filter_with_positions(const vector<Observer>& observers) -> vector<Observer> {
        return filter_vec<Observer>(observers,
                          [](const Observer& observer) -> bool {
                              return observer.has_position();
                          });
    }

    double Observer::world_to_local_units(double centimeters) const {
        return centimeters * this->_units_per_centimeter;
    }
}
