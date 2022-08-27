#ifndef RCSOP_COLMAP_OBSERVER_CAMERA_H
#define RCSOP_COLMAP_OBSERVER_CAMERA_H

#include "observer_camera.h"

namespace rcsop::common {
    class ColmapObserverCamera : public ObserverCamera {
    private:
        const vec3 _position;
        const ModelCamera _camera;
        vec3 _direction_up;

        camera_correction_transform _pitch_correction;
        camera_correction_transform _colmap_camera_rotation_fix;

        [[nodiscard]] vec3 apply_corrections(const vec3& local_point) const;

        [[nodiscard]] vec3 undo_corrections(const vec3& local_point) const;

        [[nodiscard]] vec3 get_direction_up() const;

    public:
        ColmapObserverCamera(const ModelCamera& camera, double pitch_correction);

        [[nodiscard]] vec3 map_to_observer_local(const vec3& world_coordinates, double height_offset_in_world) const override;

        [[nodiscard]] vec3 map_to_world(const vec3& observer_local, double height_offset_in_world) const override;

        [[nodiscard]] double distance_to_camera(const vec3& world_coordinates) const override;

        [[nodiscard]] ModelCamera native_camera() const override;

    };
}

#endif //RCSOP_COLMAP_OBSERVER_CAMERA_H
