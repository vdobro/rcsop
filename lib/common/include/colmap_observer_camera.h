#ifndef RCSOP_COLMAP_OBSERVER_CAMERA_H
#define RCSOP_COLMAP_OBSERVER_CAMERA_H

#include "observer_camera.h"

namespace rcsop::common {
    using camera_correction_transform = Eigen::Transform<double, 3, Eigen::Affine>;

    class ColmapObserverCamera : public ObserverCamera {
    private:
        const vec3 _position;
        const camera _camera;

        camera_correction_transform _correction;
        camera_correction_transform _colmap_camera_rotation_fix;
    public:
        explicit ColmapObserverCamera(camera camera,
                                      double pitch_correction);


        [[nodiscard]] vec3 map_to_observer_local(const vec3& world_coordinates) const override;

        [[nodiscard]] double distance_to_camera(const vec3& world_coordinates) const override;

        [[nodiscard]] camera native_camera() const override;
    };
}

#endif //RCSOP_COLMAP_OBSERVER_CAMERA_H
