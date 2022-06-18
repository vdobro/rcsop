#ifndef RCSOP_DATA_POINT_CLOUD_PROVIDER_H
#define RCSOP_DATA_POINT_CLOUD_PROVIDER_H

#include "input_data_collector.h"

class PointCloudProvider {
private:
    shared_ptr<SparseCloud> sparse_cloud = nullptr;
    ScoredPointMap sparse_cloud_points;
    point_id_t max_sparse_point_id = 0;

    shared_ptr<DenseCloud> dense_mesh = nullptr;
    vector<Vector3d> dense_cloud_points;

public:
    explicit PointCloudProvider(const InputDataCollector& input);

    [[nodiscard]] ScoredPointMap get_base_scored_points() const;

    [[nodiscard]] double get_world_scale() const;

    [[nodiscard]] vector<ScoredPoint> get_base_scored_point_list() const;
};

#endif //RCSOP_DATA_POINT_CLOUD_PROVIDER_H
