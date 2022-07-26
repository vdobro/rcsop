#include "colmap_observer_camera.h"

namespace rcsop::common {
    ColmapObserverCamera::ColmapObserverCamera(
            const camera camera,
            const double pitch_correction) :
            _position(camera.position()),
            _camera(camera) {
        const double pitch_correction_radians = (pitch_correction * M_PI) / 180.;
        this->_correction = Eigen::AngleAxis<double>(pitch_correction_radians, vec3::UnitX());

        // This is needed because in COLMAP the Y axis points downwards and not forward
        this->_colmap_camera_rotation_fix = Eigen::AngleAxis<double>(-M_PI_2, vec3::UnitX());
    }

    vec3 ColmapObserverCamera::map_to_observer_local(const vec3& world_coordinates) const {
        vec3 local_point = _camera.transform_to_local(world_coordinates);
        vec3 rotated_point = local_point.transpose() * _colmap_camera_rotation_fix.rotation() * _correction.rotation();
        return rotated_point;
    }

    double ColmapObserverCamera::distance_to_camera(const vec3& world_coordinates) const {
        return (world_coordinates - _position).norm();
    }

    camera ColmapObserverCamera::native_camera() const {
        return this->_camera;
    };
}