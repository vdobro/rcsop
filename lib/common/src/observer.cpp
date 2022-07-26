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

    struct vec3_spherical {
        double radial{};
        double azimuthal{};
        double polar{};
    };

    static vec3_spherical cartesian_to_polar(const vec3& point) {
        const double distance = point.norm();
        const double
                x = point.x(),
                y = point.y(),
                z = point.z();

        double theta;
        double phi;

        if (x > 0) {
            theta = atan(y / x);
        } else if (x == 0) {
            if (y >= 0) {
                theta = M_PI_2; // y = 0 undefined by definition, but pi/2 is okay as a fallback here
            } else { // y < 0
                theta = -M_PI_2;
            }
        } else { // x < 0
            if (y >= 0) {
                theta = atan(y / x) + M_PI;
            } else { // y < 0
                theta = atan(y / x) - M_PI;
            }
        }
        phi = (distance != 0) ? acos(z / distance) : M_PI_2;

        theta = theta * 180. / M_PI;
        phi = phi * 180. / M_PI;

        return {
                .radial = distance,
                .azimuthal = theta,
                .polar = phi,
        };
    }

    observed_point Observer::observe_point(const ScoredPoint& point) const {
        const auto world_point = point.position();
        const auto distance = _camera->distance_to_camera(world_point);
        const auto local_point = _camera->map_to_observer_local(world_point);

        const auto& [_, azimuthal, polar] = cartesian_to_polar(local_point);

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

    shared_ptr<vector<observed_point>> Observer::observe_points(
            const vector<ScoredPoint>& camera_points) const {
        auto result = map_vec_shared<ScoredPoint, observed_point, true>(
                camera_points,
                [this](const auto& point) {
                    return observe_point(point);
                });

        return result;
    }

    vec3 Observer::project_position(const observed_point& position) const {
        //TODO
        return vec3::Zero();
    }

    shared_ptr<vector<vec3>> Observer::project_observed_positions(
            const vector<observed_point>& position) const {
        auto result = map_vec_shared<observed_point, vec3, true>(
                position, [this](const auto& position) {
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
