#ifndef RCSOP_COMMON_SPARSE_CLOUD_H
#define RCSOP_COMMON_SPARSE_CLOUD_H

#include <filesystem>
#include <vector>

#include "colmap/base/reconstruction.h"

#include "camera.h"
#include "scored_point.h"

using std::filesystem::path;

class SparseCloud {
private:
    path model_path;
    std::shared_ptr<colmap::Reconstruction> reconstruction;

    std::vector<camera> cameras;
    map<camera_id_t, camera> camera_map;

public:
    explicit SparseCloud(const path& model_path);

    [[nodiscard]] double get_world_scale(double camera_distance_to_origin) const;
    [[nodiscard]] std::vector<camera> get_cameras() const;
    [[nodiscard]] camera find_camera(camera_id_t camera_id) const;

    [[nodiscard]] std::vector<Eigen::Vector3d> get_camera_positions() const;
    [[nodiscard]] std::vector<point_pair> get_point_pairs() const;
    [[nodiscard]] ScoredPointMap get_scored_points() const;

    void reload();
    void save(const path& output_path);

    void filter_points(const std::function<bool(const Vector3d&)>& predicate_to_keep);
    void add_point(const Eigen::Vector3d& point, const Eigen::Vector3ub& color);
    void set_point_color(point_id_t point_id, const Eigen::Vector3ub& color);
};

#endif //RCSOP_COMMON_SPARSE_CLOUD_H
