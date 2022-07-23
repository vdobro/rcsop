#ifndef RCSOP_COMMON_OBSERVER_H
#define RCSOP_COMMON_OBSERVER_H

#include "utils/points.h"

#include "camera.h"
#include "observer_position.h"
#include "observed_point.h"

namespace rcsop::common {
    using rcsop::common::utils::points::vec3;
    using plane = Eigen::Hyperplane<double, 3>;

    struct camera_options {
        double pitch_correction{};
        double distance_to_origin{};
        height_t default_height{};
    };

    struct observer_position_props {
        vec3 observer_position;
        vec3 direction_right;
        vec3 direction_up;
        plane vertical_plane;
        plane horizontal_plane;
    };

    class Observer {
    private:
        optional<ObserverPosition> _position;
        path _source_filepath;
        camera _camera;

        double _units_per_centimeter = 1;
        Eigen::Transform<double, 3, Eigen::Affine> _correction_transform;

        [[nodiscard]] vec3 get_right() const;

        [[nodiscard]] vec3 get_up() const;

        [[nodiscard]] observed_point
        observe_point(const ScoredPoint& point,
                      const observer_position_props& observer_position_props) const;

        [[nodiscard]] vec3
        project_position(const observed_point& position,
                         const observer_position_props& observer_position_props) const;

        [[nodiscard]] observer_position_props get_observer_props() const;

    public:
        explicit Observer(optional<ObserverPosition> camera_position,
                          path filepath,
                          camera camera,
                          camera_options options);

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
