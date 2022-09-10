#ifndef RCSOP_DATA_POINT_CLOUD_PROVIDER_H
#define RCSOP_DATA_POINT_CLOUD_PROVIDER_H

#include "utils/points.h"
#include "observer.h"

#include "input_data_collector.h"

namespace rcsop::data {
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point = Kernel::Point_3;
    using BoundingBox = Kernel::Iso_cuboid_3;

    using rcsop::common::utils::points::point_id_t;
    using rcsop::common::utils::points::vec3;

    using rcsop::common::SparseCloud;
    using rcsop::common::SimplePoint;
    using rcsop::common::DenseCloud;
    using rcsop::common::camera_options;

    enum ReconstructionType {
        SPARSE_CLOUD = 1 << 0,
        DENSE_CLOUD = 1 << 1,
        COMPLETE = SPARSE_CLOUD | DENSE_CLOUD,
    };

    class PointCloudProvider {
    private:
        shared_ptr<SparseCloud> sparse_cloud = nullptr;
        shared_ptr<vector<SimplePoint>> sparse_cloud_points = make_shared<vector<SimplePoint>>();
        point_id_t max_sparse_point_id = 0;

        shared_ptr<DenseCloud> dense_mesh = nullptr;
        shared_ptr<vector<SimplePoint>> dense_cloud_points = make_shared<vector<SimplePoint>>();

        BoundingBox _bounding_box;

        double _distance_to_origin = 1.;
        double _units_per_centimeter = 1.;


    public:
        explicit PointCloudProvider(const InputDataCollector& input,
                                    const camera_options& camera_options);

        [[nodiscard]] auto get_base_points(ReconstructionType cloud_selection = ReconstructionType::COMPLETE,
                                           size_t take_every_nth = 1) const -> shared_ptr<vector<SimplePoint>>;

        [[nodiscard]] auto generate_homogenous_cloud(size_t points_per_meter) const -> shared_ptr<vector<SimplePoint>>;

        [[nodiscard]] auto get_bounding_box() const -> BoundingBox;
    };
}

#endif //RCSOP_DATA_POINT_CLOUD_PROVIDER_H
