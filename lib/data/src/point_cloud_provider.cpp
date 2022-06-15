#include "point_cloud_provider.h"

using std::make_pair;

PointCloudProvider::PointCloudProvider(const InputDataCollector& input) {
    if (input.data_available<SPARSE_CLOUD_COLMAP>()) {
        this->sparse_cloud = input.data<SPARSE_CLOUD_COLMAP>(false);
        this->sparse_cloud_points = sparse_cloud->get_scored_points();

        auto point_ids = map_vec<point_pair, point_id_t>(
                sparse_cloud->get_point_pairs(),
                [](const point_pair& point_pair) -> point_id_t {
                    return point_pair.first;
                });
        max_sparse_point_id = *std::max_element(point_ids.begin(), point_ids.end());
    }
    if (input.data_available<DENSE_MESH_PLY>()) {
        this->dense_mesh = input.data<DENSE_MESH_PLY>(false);
        this->dense_cloud_points = this->dense_mesh->points();
    }
}

scored_point_map PointCloudProvider::get_base_scored_points() const {
    scored_point_map result;
    for (const auto& point: sparse_cloud_points) {
        result.insert(make_pair(point.first, scored_point(point.second)));
    }

    auto dense_point_id = max_sparse_point_id + 1;
    for (const auto& point: dense_cloud_points) {
        result.insert(make_pair(dense_point_id, scored_point(point, dense_point_id, 0)));
        dense_point_id++;
    }

    return result;
}

double PointCloudProvider::get_world_scale() const {
    return this->sparse_cloud->get_world_scale(750.0);
}
