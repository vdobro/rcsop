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
        bool use_any_camera_nearby{false};
    };

    struct data_observer_translation {
        double roll{};
    };

    struct observer_camera_options {
        height_t height_offset;
    };

    class Observer {
    private:
        optional<ObserverPosition> _position;
        path _source_filepath;
        shared_ptr<ObserverCamera const> _camera;
        data_observer_translation _observer_translation;
        observer_camera_options _camera_options;

        double _units_per_centimeter = 1;

        [[nodiscard]] vec3 translate_data_point(const vec3& local_point) const;

        [[nodiscard]] double get_height_offset() const;

    public:
        Observer(optional<ObserverPosition> camera_position,
                 path filepath,
                 shared_ptr<ObserverCamera const> observer_camera,
                 data_observer_translation observer_translation = {},
                 observer_camera_options camera_options = {});

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
        project_position(const observed_point& position) const -> vec3;

        [[nodiscard]] auto
        project_observed_positions(const vector<observed_point>& position) const -> shared_ptr<vector<vec3>>;

        [[nodiscard]] auto clone_with_data(data_observer_translation data_options) const -> Observer;

        [[nodiscard]] auto clone_with_camera(observer_camera_options observer_options) const -> Observer;

        [[nodiscard]] auto clone_with_position(ObserverPosition position) const -> Observer;

        [[nodiscard]] static auto cartesian_to_spherical(const vec3& point) -> vec3_spherical;

        [[nodiscard]] static auto spherical_to_cartesian(const vec3_spherical& point) -> vec3;

        [[nodiscard]] static auto map_to_positions(const vector<Observer>& observers) -> set<ObserverPosition>;

        [[nodiscard]] static auto filter_with_positions(const vector<Observer>& observers) -> vector<Observer>;
    };
}

#endif //RCSOP_COMMON_OBSERVER_H
