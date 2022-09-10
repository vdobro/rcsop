#include "sparse_filter.h"

#include "utils/types.h"
#include "utils/mapping.h"

#include "observer_provider.h"

namespace rcsop::launcher::tasks {
    using rcsop::common::Observer;
    using rcsop::common::SparseCloud;
    using rcsop::common::ModelCamera;
    using rcsop::common::camera_options;

    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::min_value;
    using rcsop::common::utils::max_value;
    using rcsop::common::utils::points::vec3;

    using rcsop::data::ObserverProvider;
    using rcsop::data::SPARSE_CLOUD_COLMAP;

    static double distance_from_origin(const vec3& point, const vec3& origin) {
        return (point - origin).norm();
    }

    static double max_camera_distance(const InputDataCollector& inputs,
                                      const camera_options& camera_options) {
        auto observer_provider = make_unique<ObserverProvider>(inputs, camera_options);
        auto observers = observer_provider->observers_with_positions();
        if (observers.empty()) {
            throw std::invalid_argument("No observers found");
        }
        auto cameras = map_vec<Observer, ModelCamera>(observers, &Observer::native_camera);
        auto positions = map_vec<ModelCamera, vec3>(cameras, &ModelCamera::position);

        const auto origin = vec3::Zero(); //TODO: filter outliers and approximate the average origin point (iteratively)
        auto distances = map_vec<vec3, double>(positions, [origin](const vec3& point) {
            return distance_from_origin(point, origin);
        });
        auto max = max_value(distances);
        auto min = min_value(distances);
        return std::min(min * 1.5, max);
    }

    void sparse_filter(const InputDataCollector& inputs,
                       const task_options& options) {
        auto distance_threshold = max_camera_distance(inputs, options.camera);
        shared_ptr<SparseCloud> model = inputs.data<SPARSE_CLOUD_COLMAP>();

        auto origin = vec3::Zero(); //TODO: filter outliers and approximate the average origin point (iteratively)
        model->filter_points([distance_threshold, origin](const vec3& point) -> bool {
            return distance_from_origin(point, origin) <= distance_threshold;
        });
        model->save(options.output_path);
    }
}

