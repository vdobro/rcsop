#include "sparse_filter.h"

#include "utils/types.h"
#include "utils/mapping.h"

#include "observer_provider.h"

static double distance_from_origin(const Vector3d& point) {
    return point.norm();
}

static double max_camera_distance(const InputDataCollector& inputs,
                                  const camera_options& camera_options) {
    auto observer_provider = make_unique<ObserverProvider>(inputs, camera_options);
    auto observers = observer_provider->observers_with_positions();
    if (observers.empty()) {
        throw std::invalid_argument("No observers found");
    }
    auto cameras = map_vec<Observer, camera>(observers, &Observer::native_camera);
    auto positions = map_vec<camera, Vector3d>(cameras, &camera::position);
    auto distances = map_vec<Vector3d, double>(positions, distance_from_origin);
    return *std::max_element(distances.begin(), distances.end());
}

using rcsop::launcher::utils::task_options;
void rcsop::launcher::tasks::sparse_filter(const InputDataCollector& inputs,
                                           const task_options& options) {
    auto distance_threshold = 1.1 * max_camera_distance(inputs, options.camera);
    shared_ptr<SparseCloud> model = inputs.data<SPARSE_CLOUD_COLMAP>(false);
    model->filter_points([distance_threshold](const Vector3d& point) -> bool {
        return distance_from_origin(point) <= distance_threshold;
    });
    model->save(options.output_path);
}
