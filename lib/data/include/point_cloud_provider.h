#ifndef RCSOP_DATA_POINT_CLOUD_PROVIDER_H
#define RCSOP_DATA_POINT_CLOUD_PROVIDER_H

#include "utils/points.h"
#include "observer.h"

#include "input_data_collector.h"

namespace rcsop::data {
    using rcsop::common::utils::points::point_id_t;
    using rcsop::common::utils::points::vec3;

    using rcsop::common::SparseCloud;
    using rcsop::common::ScoredPoint;
    using rcsop::common::DenseCloud;
    using rcsop::common::camera_options;

    class PointCloudProvider {
    private:
        shared_ptr<SparseCloud> sparse_cloud = nullptr;
        shared_ptr<vector<ScoredPoint>> sparse_cloud_points = make_shared<vector<ScoredPoint>>();
        point_id_t max_sparse_point_id = 0;

        shared_ptr<DenseCloud> dense_mesh = nullptr;
        shared_ptr<vector<vec3>> dense_cloud_points = make_shared<vector<vec3>>();

        double _distance_to_origin = 1.;
        double _units_per_centimeter = 1.;
    public:
        explicit PointCloudProvider(const InputDataCollector& input,
                                    const camera_options& camera_options);

        [[nodiscard]] shared_ptr<vector<ScoredPoint>> get_base_scored_points(size_t take_every_nth = 1) const;

        [[nodiscard]] shared_ptr<vector<ScoredPoint>> generate_homogenous_cloud(size_t points_per_meter) const;
    };
}

#endif //RCSOP_DATA_POINT_CLOUD_PROVIDER_H
