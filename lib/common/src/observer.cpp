#include "observer.h"

#include <ranges>
#include <utility>

#include "utils/mapping.h"

#include "observed_point.h"

namespace rcsop::common {
    using rcsop::common::utils::map_vec_shared;
    using rcsop::common::utils::filter_vec;

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

    auto Observer::spherical_to_cartesian(const vec3_spherical& point) -> vec3 {
        const auto& [radial, azimuthal, polar] = point;
        const auto phi = azimuthal * M_PI / 180.;
        const auto theta = polar * M_PI / 180.;

        const auto x = radial * cos(phi) * sin(theta);
        const auto y = radial * sin(phi) * sin(theta);
        const auto z = radial * cos(theta);
        return {x, y, z};
    }

    auto Observer::observe_point(const ScoredPoint& point) const -> observed_point {
        const auto world_point = point.position();
        const auto distance = _camera->distance_to_camera(world_point);
        const auto local_point = _camera->map_to_observer_local(world_point, get_height_offset());

        //auto point_check = _camera->map_to_world(local_point);
        //assert(point_check == world_point);

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

    auto Observer::translate_data_point(const vec3& local_point) const -> vec3 {
        camera_correction_transform world_roll;
        world_roll = Eigen::AngleAxis<double>(
                (_observer_translation.roll * M_PI) / 180., vec3::UnitY());
        return local_point.transpose() * world_roll.rotation();
    }

    auto Observer::project_position(const observed_point& observed_point) const -> vec3 {
        const auto& [_p, _id, distance_in_world, vertical_angle, horizontal_angle] = observed_point;
        if (distance_in_world == 0) {
            return this->_camera->native_camera().position();
        }
        auto radial = distance_in_world * this->_units_per_centimeter;
        auto azimuthal = -(horizontal_angle - 90);
        auto polar = 90 - vertical_angle;

        auto cartesian_local = spherical_to_cartesian({.radial = radial, .azimuthal = azimuthal, .polar = polar});
        auto translated_local = translate_data_point(cartesian_local);

        return _camera->map_to_world(translated_local, get_height_offset());
    }

    auto
    Observer::observe_points(const vector<ScoredPoint>& camera_points) const -> shared_ptr<vector<observed_point>> {
        auto result = map_vec_shared<ScoredPoint, observed_point, true>(
                camera_points,
                [this](const auto& point) {
                    return observe_point(point);
                });

        return result;
    }

    auto
    Observer::project_observed_positions(const vector<observed_point>& positions) const -> shared_ptr<vector<vec3>> {
        auto result = map_vec_shared<observed_point, vec3, true>(
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

    auto Observer::filter_with_positions(const vector<Observer>& observers) -> vector<Observer> {
        return filter_vec<Observer>(observers,
                          [](const Observer& observer) -> bool {
                              return observer.has_position();
                          });
    }
}
