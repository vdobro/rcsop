#include "sparse_filter.h"

#include "utils/types.h"
#include "utils/mapping.h"

#include "observer_provider.h"

namespace rcsop::launcher::tasks {
    using rcsop::common::Observer;
    using rcsop::common::SparseCloud;
    using rcsop::common::camera;
    using rcsop::common::camera_options;

    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::points::vec3;

    using rcsop::data::ObserverProvider;
    using rcsop::data::SPARSE_CLOUD_COLMAP;

    static double distance_from_origin(const vec3& point) {
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
        auto positions = map_vec<camera, vec3>(cameras, &camera::position);
        auto distances = map_vec<vec3, double>(positions, distance_from_origin);
        return *std::max_element(distances.begin(), distances.end());
    }

    void sparse_filter(const InputDataCollector& inputs,
                       const task_options& options) {
        auto distance_threshold = 1.1 * max_camera_distance(inputs, options.camera);
        shared_ptr<SparseCloud> model = inputs.data<SPARSE_CLOUD_COLMAP>(false);
        model->filter_points([distance_threshold](const vec3& point) -> bool {
            return distance_from_origin(point) <= distance_threshold;
        });
        model->save(options.output_path);
    }
}

