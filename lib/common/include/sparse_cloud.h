#ifndef RCSOP_COMMON_SPARSE_CLOUD_H
#define RCSOP_COMMON_SPARSE_CLOUD_H

#include "colmap/base/reconstruction.h"

#include "utils/types.h"
#include "utils/points.h"
#include "utils/sparse.h"

#include "camera.h"
#include "scored_point.h"

namespace rcsop::common {
    using rcsop::common::utils::sparse::Reconstruction;
    using rcsop::common::utils::sparse::color_vec;
    using rcsop::common::utils::points::point_pair;

    class SparseCloud {
    private:
        path model_path;
        shared_ptr<Reconstruction> reconstruction;

        vector<camera> cameras;
        map<camera_id_t, camera> camera_map;

    public:
        explicit SparseCloud(const path& model_path);

        [[nodiscard]] vector<camera> get_cameras() const;

        [[nodiscard]] vector<point_pair> get_point_pairs() const;

        [[nodiscard]] shared_ptr<vector<ScoredPoint>> get_scored_points() const;

        void reload();

        void save(const path& output_path);

        void filter_points(const function<bool(const Vector3d&)>& predicate_to_keep);

        void add_point(const Vector3d& point, const color_vec& color);

        void set_point_color(point_id_t point_id, const color_vec& color);
    };

}

#endif //RCSOP_COMMON_SPARSE_CLOUD_H
