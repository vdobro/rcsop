#include "point_cloud_provider.h"

#include <fstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/bounding_box.h>

#include "utils/mapping.h"
#include "utils/chronometer.h"
#include "observer_provider.h"
#include "utils/random.h"

namespace rcsop::data {
    using rcsop::common::utils::points::point_pair;
    using rcsop::common::utils::time::start_time;
    using rcsop::common::utils::time::log_and_start_next;
    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::get_uniform_distribution;

    PointCloudProvider::PointCloudProvider(const InputDataCollector& input,
                                           const camera_options& camera_options)
            : _distance_to_origin(camera_options.distance_to_origin) {
        if (input.data_available<SPARSE_CLOUD_COLMAP>()) {
            this->sparse_cloud = input.data<SPARSE_CLOUD_COLMAP>();
            this->sparse_cloud_points = sparse_cloud->get_points();

            auto point_ids = map_vec<SimplePoint, point_id_t>(*sparse_cloud_points, &SimplePoint::id);
            max_sparse_point_id = *std::max_element(point_ids.begin(), point_ids.end());

            auto observer_provider = ObserverProvider(input, camera_options, false);
            this->_units_per_centimeter = observer_provider.get_units_per_centimeter();
        }
        if (input.data_available<DENSE_MESH_PLY>()) {
            this->dense_mesh = input.data<DENSE_MESH_PLY>();
            this->dense_cloud_points = this->dense_mesh->get_points();
        }
        _bounding_box = get_bounding_box();
    }

    auto PointCloudProvider::get_bounding_box() const -> BoundingBox {
        auto base_points = make_shared<vector<Point>>();
        auto raw_points = get_base_points();
        for (const auto& point: *raw_points) {
            const auto point_pos = point.position();
            base_points->emplace_back(point_pos.x(), point_pos.y(), point_pos.z());
        }
        return CGAL::bounding_box(base_points->cbegin(), base_points->cend());
    }

    auto PointCloudProvider::get_base_points(ReconstructionType cloud_selection,
                                             size_t take_every_nth) const -> shared_ptr<vector<SimplePoint>> {
        assert(take_every_nth >= 1);

        auto result = make_shared<vector<SimplePoint>>();

        if ((cloud_selection & ReconstructionType::SPARSE_CLOUD) != 0) {
            std::copy(sparse_cloud_points->cbegin(), sparse_cloud_points->cend(), std::back_inserter(*result));
        }
        if ((cloud_selection & ReconstructionType::DENSE_CLOUD) != 0) {
            auto dense_point_id = max_sparse_point_id + 1;
            for (const auto& point: *dense_cloud_points) {
                result->emplace_back(dense_point_id, point.position());
                dense_point_id++;
            }
        }

        if (take_every_nth == 1) return result;

        auto filtered_result = make_shared<vector<SimplePoint>>();
        for (size_t i = 0; i < result->size(); i += take_every_nth) {
            filtered_result->push_back(result->at(i));
        }
        return filtered_result;
    }

    auto PointCloudProvider::generate_homogenous_cloud(size_t points_per_meter) const -> shared_ptr<vector<SimplePoint>> {
        const double units_per_meter = _units_per_centimeter * 100;
        const double step_size_meters = 1. / static_cast<double>(points_per_meter);
        const double step_size = step_size_meters * units_per_meter;

        const vec3 middle_point = vec3::Zero();
        const double radius_limit = _distance_to_origin * _units_per_centimeter * 1.5;
        const double begin_x = std::max(middle_point.x() - radius_limit, _bounding_box.xmin());
        const double begin_y = std::max(middle_point.y() - radius_limit, _bounding_box.ymin());
        const double begin_z = std::max(middle_point.z() - radius_limit, _bounding_box.zmin());

        const double end_x = std::min(middle_point.x() + radius_limit, _bounding_box.xmax());
        const double end_y = std::min(middle_point.y() + radius_limit, _bounding_box.ymax());
        const double end_z = std::min(middle_point.z() + radius_limit, _bounding_box.zmax());

        const double span_x = (end_x - begin_x);
        const double span_y = (end_y - begin_y);
        const double span_z = (end_z - begin_z);

        auto result = make_shared<vector<SimplePoint>>();
        size_t point_count = ceil(
                (span_x / step_size - 1)
                * (span_y / step_size - 1)
                * (span_z / step_size - 1));

        std::clog << "A maximum of " << std::to_string(point_count)
                  << " points to be generated" << std::endl;

        auto dist = get_uniform_distribution(step_size, true);

        const auto distance_threshold = radius_limit;
        size_t index = 0;
        auto x = begin_x;
        auto time = start_time();
        while (x < end_x) {
            auto y = begin_y;
            while (y < end_y) {
                auto z = begin_z;
                while (z < end_z) {
                    const vec3 point_vector(x + dist(), y + dist(), z + dist());
                    const double distance_from_origin = (middle_point - point_vector).norm();
                    if (distance_from_origin <= distance_threshold) {
                        result->emplace_back(index, point_vector);

                        index++;
                    }
                    z += step_size;
                }
                y += step_size;
            }
            x += step_size;
        }

        log_and_start_next(time, "Generated " + std::to_string(index + 1) + " point cloud");
        return result;
    }
}
