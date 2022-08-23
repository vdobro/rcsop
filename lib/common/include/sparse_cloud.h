#ifndef RCSOP_COMMON_SPARSE_CLOUD_H
#define RCSOP_COMMON_SPARSE_CLOUD_H

#include "colmap/base/reconstruction.h"

#include "utils/types.h"
#include "utils/points.h"
#include "utils/sparse.h"

#include "camera.h"
#include "scored_point.h"
#include "base_point_cloud.h"

namespace rcsop::common {
    using rcsop::common::utils::sparse::Reconstruction;
    using rcsop::common::utils::sparse::color_vec;
    using rcsop::common::utils::points::point_pair;
    using rcsop::common::BasePointCloud;

    class SparseCloud : public BasePointCloud {
    private:
        shared_ptr<Reconstruction> reconstruction;
        vector<camera> cameras;

    public:
        explicit SparseCloud(const path& model_path);

        [[nodiscard]] vector<camera> get_cameras() const;

        [[nodiscard]] shared_ptr<vector<IdPoint>> get_points() const override;

        [[nodiscard]] size_t point_count() const override;

        void reload();

        void save(const path& output_path);

        void filter_points(const function<bool(const vec3&)>& predicate_to_keep) override;

        void add_point(const vec3& point, const color_vec& color) override;

        void write(const path& output_path) override;

        void purge_cameras(camera_id_t camera_to_keep) override;

        void set_point_color(point_id_t point_id, const color_vec& color);
    };

}

#endif //RCSOP_COMMON_SPARSE_CLOUD_H
