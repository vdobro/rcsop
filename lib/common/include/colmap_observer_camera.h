#ifndef RCSOP_COLMAP_OBSERVER_CAMERA_H
#define RCSOP_COLMAP_OBSERVER_CAMERA_H

#include "observer_camera.h"

namespace rcsop::common {
    using camera_correction_transform = Eigen::Transform<double, 3, Eigen::Affine>;

    class ColmapObserverCamera : public ObserverCamera {
    private:
        const vec3 _position;
        const ModelCamera _camera;

        camera_correction_transform _pitch_correction;
        camera_correction_transform _colmap_camera_rotation_fix;
    public:
        explicit ColmapObserverCamera(const ModelCamera& camera,
                                      double pitch_correction);


        [[nodiscard]] vec3 map_to_observer_local(const vec3& world_coordinates) const override;

        [[nodiscard]] vec3 map_to_world(const vec3& observer_local) const override;

        [[nodiscard]] double distance_to_camera(const vec3& world_coordinates) const override;

        [[nodiscard]] ModelCamera native_camera() const override;
    };
}

#endif //RCSOP_COLMAP_OBSERVER_CAMERA_H
