#ifndef RCSOP_DATA_POINT_CLOUD_PROVIDER_H
#define RCSOP_DATA_POINT_CLOUD_PROVIDER_H

#include "input_data_collector.h"

class PointCloudProvider {
private:
    shared_ptr<SparseCloud> sparse_cloud = nullptr;
    shared_ptr<vector<ScoredPoint>> sparse_cloud_points = make_shared<vector<ScoredPoint>>();
    point_id_t max_sparse_point_id = 0;

    shared_ptr<DenseCloud> dense_mesh = nullptr;
    shared_ptr<vector<Vector3d>> dense_cloud_points = make_shared<vector<Vector3d>>();

    double _distance_to_origin = 1.;
    double _units_per_centimeter = 1.;
public:
    explicit PointCloudProvider(const InputDataCollector& input,
                                const camera_options& camera_options);

    [[nodiscard]] shared_ptr<vector<ScoredPoint>> get_base_scored_points() const;

    [[nodiscard]] shared_ptr<vector<ScoredPoint>> generate_homogenous_cloud(size_t points_per_meter) const;
};

#endif //RCSOP_DATA_POINT_CLOUD_PROVIDER_H
