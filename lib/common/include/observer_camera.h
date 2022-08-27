#ifndef RCSOP_COMMON_OBSERVER_CAMERA_H
#define RCSOP_COMMON_OBSERVER_CAMERA_H

#include "model_camera.h"

namespace rcsop::common {
    using rcsop::common::utils::points::vec3;
    using camera_correction_transform = Eigen::Transform<double, 3, Eigen::Affine>;

    class ObserverCamera {
    public:
        virtual ~ObserverCamera() = default;

        [[nodiscard]] virtual vec3 map_to_observer_local(const vec3& world_coordinates,
                                                         double height_offset_in_world) const = 0;

        [[nodiscard]] virtual vec3 map_to_world(const vec3& observer_local,
                                                double height_offset_in_world) const = 0;

        [[nodiscard]] virtual double distance_to_camera(const vec3& world_coordinates) const = 0;

        [[nodiscard]] virtual ModelCamera native_camera() const = 0;
    };
}

#endif //RCSOP_COMMON_OBSERVER_CAMERA_H
