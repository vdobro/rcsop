#ifndef RCSOP_COMMON_OBSERVER_H
#define RCSOP_COMMON_OBSERVER_H

#include "utils/points.h"

#include "model_camera.h"
#include "observer_position.h"
#include "observed_point.h"
#include "observer_camera.h"

namespace rcsop::common {
    using rcsop::common::utils::points::vec3;
    using rcsop::common::utils::points::vec3_spherical;

    struct camera_options {
        double pitch_correction{};
        double distance_to_origin{};
        height_t default_height{};
    };

    struct data_observer_translation {
        double vertical_offset{};
        double rotation{};
    };

    class Observer {
    private:
        optional<ObserverPosition> _position;
        path _source_filepath;
        shared_ptr<ObserverCamera const> _camera;

        double _units_per_centimeter = 1;
    public:
        explicit Observer(optional<ObserverPosition> camera_position,
                          path filepath,
                          shared_ptr<ObserverCamera const> observer_camera);

        virtual ~Observer() = default;

        void set_units_per_centimeter(double units);

        [[nodiscard]] auto position() const -> ObserverPosition;

        [[nodiscard]] auto has_position() const -> bool;

        [[nodiscard]] auto source_image_path() const -> path;

        [[nodiscard]] auto native_camera() const -> ModelCamera;

        [[nodiscard]] auto observe_point(const ScoredPoint& point) const -> observed_point;

        [[nodiscard]] auto
        observe_points(const vector<ScoredPoint>& camera_points) const -> shared_ptr<vector<observed_point>>;

        [[nodiscard]] auto
        project_position(const observed_point& position,
                         const data_observer_translation& observer_translation = {}) const -> vec3;

        [[nodiscard]] auto
        project_observed_positions(const vector<observed_point>& position,
                                   const data_observer_translation& observer_translation = {}) const -> shared_ptr<vector<vec3>>;

        [[nodiscard]] static auto cartesian_to_spherical(const vec3& point) -> vec3_spherical;

        [[nodiscard]] static auto spherical_to_cartesian(const vec3_spherical& point) -> vec3;
    };
}

#endif //RCSOP_COMMON_OBSERVER_H
