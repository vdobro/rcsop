#include "cleanup.h"

#include "../../common/utils/vector.h"

static double distance_from_origin(const Vector3d& point) {
    return point.norm();
}

static double max_camera_distance(const sparse_cloud& model) {
    auto image_positions = map_vec<camera, Vector3d>(model.get_cameras(), [](const camera& camera) {
        return camera.get_position();
    });
    auto distances = map_vec<Vector3d, double>(image_positions, distance_from_origin);
    return *std::max_element(distances.begin(), distances.end());
}

void filter_points(sparse_cloud& model,
                   const path& output_path) {
    auto distance_threshold = max_camera_distance(model);

    model.filter_points([distance_threshold](const Vector3d& point) {
        return distance_from_origin(point) <= distance_threshold * 0.9;
    });
    model.save(output_path);
}