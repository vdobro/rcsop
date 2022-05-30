#ifndef SFM_COLORING_COMMON_SPARSE_CLOUD_H
#define SFM_COLORING_COMMON_SPARSE_CLOUD_H

#include <filesystem>
#include <vector>

#include "colmap/base/reconstruction.h"

#include "camera.h"
#include "scored_point.h"

class sparse_cloud {
private:
    std::filesystem::path model_path;
    std::unique_ptr<colmap::Reconstruction> reconstruction;

    std::vector<camera> cameras;
    map<camera_id_t, camera> camera_map;

public:
    explicit sparse_cloud(const std::filesystem::path& model_path);

    [[nodiscard]] double get_world_scale(double camera_distance_to_origin) const;
    [[nodiscard]] std::vector<camera> get_cameras() const;
    [[nodiscard]] camera find_camera(camera_id_t camera_id) const;

    [[nodiscard]] std::vector<Eigen::Vector3d> get_camera_positions() const;
    [[nodiscard]] std::vector<point_pair> get_point_pairs() const;
    [[nodiscard]] scored_point_map get_scored_points() const;

    void reload();
    void save(const std::filesystem::path& output_path);

    void filter_points(const std::function<bool(const Vector3d&)>& predicate_to_keep);
    void add_point(const Eigen::Vector3d& point, const Eigen::Vector3ub& color);
    void set_point_color(point_id_t point_id, const Eigen::Vector3ub& color);
};

#endif //SFM_COLORING_COMMON_SPARSE_CLOUD_H