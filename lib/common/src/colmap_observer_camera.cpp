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

    vec3 ColmapObserverCamera::map_to_world(const vec3& observer_local) const {
        auto camera_test_to_world = _camera.transform_to_world(observer_local);
        auto camera_test_to_local = _camera.transform_to_local(camera_test_to_world);
        assert(camera_test_to_local == observer_local);
        vec3 local_point = observer_local.transpose()
                           * _correction.rotation().transpose()
                           * _colmap_camera_rotation_fix.rotation().transpose();
        return _camera.transform_to_world(local_point);
    }

    double ColmapObserverCamera::distance_to_camera(const vec3& world_coordinates) const {
        return (world_coordinates - _position).norm();
    }

    camera ColmapObserverCamera::native_camera() const {
        return this->_camera;
    };
}