#include "colmap_observer_camera.h"

namespace rcsop::common {
    ColmapObserverCamera::ColmapObserverCamera(
            const ModelCamera& camera,
            const double pitch_correction) :
            _position(camera.position()),
            _camera(camera) {
        this->_pitch_correction = Eigen::AngleAxis<double>((pitch_correction * M_PI) / 180., vec3::UnitX());

        // This is needed because in COLMAP the Y axis points downwards and not forward
        this->_colmap_camera_rotation_fix = Eigen::AngleAxis<double>(M_PI_2, vec3::UnitX());

        this->_direction_up = get_direction_up();
    }

    vec3 ColmapObserverCamera::apply_corrections(const vec3& local_point) const {
        return local_point.transpose()
               * _colmap_camera_rotation_fix.rotation()
               * _pitch_correction.rotation();
    }

    vec3 ColmapObserverCamera::undo_corrections(const vec3& local_point) const {
        return local_point.transpose()
               * _pitch_correction.rotation().transpose()
               * _colmap_camera_rotation_fix.rotation().transpose();
    }

    vec3 ColmapObserverCamera::get_direction_up() const {
        vec3 camera_up;
        camera_up.setZero();
        camera_up.y() = -1;
        vec3 direction = _camera.transform_to_world(camera_up.transpose() * _pitch_correction.rotation()) - _camera.position();
        return direction.normalized();
    }

    vec3 ColmapObserverCamera::map_to_observer_local(const vec3& world_coordinates,
                                                     double height_offset_in_world) const {
        vec3 local_point = _camera.transform_to_local(world_coordinates);
        vec3 rotated_point = apply_corrections(local_point) - (_direction_up * height_offset_in_world);
        return rotated_point;
    }

    vec3 ColmapObserverCamera::map_to_world(const vec3& observer_local,
                                            double height_offset_in_world) const {
        //vec3 camera_test_to_world = _camera.transform_to_world(observer_local);
        //vec3 camera_test_to_local = _camera.transform_to_local(camera_test_to_world);
        //assert_near(observer_local, camera_test_to_local);
        vec3 local_point = undo_corrections(observer_local);
        vec3 offset = _direction_up * height_offset_in_world;
        vec3 result = _camera.transform_to_world(local_point) + offset;
        //assert_near(this->map_to_observer_local(result), observer_local);
        return result;
    }

    double ColmapObserverCamera::distance_to_camera(const vec3& world_coordinates) const {
        return (world_coordinates - _position).norm();
    }

    ModelCamera ColmapObserverCamera::native_camera() const {
        return this->_camera;
    };
}