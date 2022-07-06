#ifndef RCSOP_COMMON_SPARSE_CLOUD_H
#define RCSOP_COMMON_SPARSE_CLOUD_H

#include "colmap/base/reconstruction.h"

#include "utils/types.h"
#include "utils/points.h"
#include "utils/colmap.h"

#include "camera.h"
#include "scored_point.h"

class SparseCloud {
private:
    path model_path;
    shared_ptr<Reconstruction> reconstruction;

    vector<camera> cameras;
    map<camera_id_t, camera> camera_map;

public:
    explicit SparseCloud(const path& model_path);

    [[nodiscard]] double get_units_per_centimeter(double camera_distance_to_origin) const;
    [[nodiscard]] vector<camera> get_cameras() const;

    [[nodiscard]] vector<Vector3d> get_camera_positions() const;
    [[nodiscard]] vector<point_pair> get_point_pairs() const;
    [[nodiscard]] shared_ptr<vector<ScoredPoint>> get_scored_points() const;

    void reload();
    void save(const path& output_path);

    void filter_points(const function<bool(const Vector3d&)>& predicate_to_keep);
    void add_point(const Vector3d& point, const Vector3ub& color);
    void set_point_color(point_id_t point_id, const Vector3ub& color);
};

#endif //RCSOP_COMMON_SPARSE_CLOUD_H
