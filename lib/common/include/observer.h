#ifndef RCSOP_COMMON_OBSERVER_H
#define RCSOP_COMMON_OBSERVER_H

#include "utils/points.h"

#include "camera.h"
#include "observer_position.h"
#include "observed_point.h"
#include "observer_camera.h"

namespace rcsop::common {
    using rcsop::common::utils::points::vec3;

    struct camera_options {
        double pitch_correction{};
        double distance_to_origin{};
        height_t default_height{};
    };

    class Observer {
    private:
        optional<ObserverPosition> _position;
        path _source_filepath;
        shared_ptr<ObserverCamera const> _camera;

        double _units_per_centimeter = 1;

        [[nodiscard]] observed_point observe_point(const ScoredPoint& point) const;

        [[nodiscard]] vec3 project_position(const observed_point& position) const;

    public:
        explicit Observer(optional<ObserverPosition> camera_position,
                          path filepath,
                          shared_ptr<ObserverCamera const> observer_camera);

        virtual ~Observer() = default;

        void set_units_per_centimeter(double units);

        [[nodiscard]] ObserverPosition position() const;

        [[nodiscard]] bool has_position() const;

        [[nodiscard]] path source_image_path() const;

        [[nodiscard]] camera native_camera() const;

        [[nodiscard]] shared_ptr<vector<observed_point>>
        observe_points(const vector<ScoredPoint>& camera_points) const;

        [[nodiscard]] shared_ptr<vector<vec3>>
        project_observed_positions(const vector<observed_point>& position) const;
    };
}

#endif //RCSOP_COMMON_OBSERVER_H
