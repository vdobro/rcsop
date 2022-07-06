#include "point_cloud_provider.h"

#include <fstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/bounding_box.h>

#include "utils/mapping.h"
#include "utils/chronometer.h"

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_3;
using SurfaceMesh = CGAL::Surface_mesh<Point>;

PointCloudProvider::PointCloudProvider(const InputDataCollector& input,
                                       double distance_to_origin)
        : _distance_to_origin(distance_to_origin) {
    if (input.data_available<SPARSE_CLOUD_COLMAP>()) {
        this->sparse_cloud = input.data<SPARSE_CLOUD_COLMAP>(false);
        this->sparse_cloud_points = sparse_cloud->get_scored_points();

        auto point_ids = map_vec<point_pair, point_id_t>(
                sparse_cloud->get_point_pairs(), &point_pair::first);
        max_sparse_point_id = *std::max_element(point_ids.begin(), point_ids.end());
    }
    if (input.data_available<DENSE_MESH_PLY>()) {
        this->dense_mesh = input.data<DENSE_MESH_PLY>(false);
        this->dense_cloud_points = this->dense_mesh->points();
    }
}

shared_ptr<vector<ScoredPoint>> PointCloudProvider::get_base_scored_points() const {
    auto result = make_shared<vector<ScoredPoint>>(*sparse_cloud_points);
    auto dense_point_id = max_sparse_point_id + 1;
    for (const auto& point: *dense_cloud_points) {
        result->emplace_back(point, dense_point_id, 0);
        dense_point_id++;
    }

    return result;
}

shared_ptr<vector<ScoredPoint>> PointCloudProvider::generate_homogenous_cloud(
        size_t points_per_meter) const {
    auto base_points = make_shared<vector<Point>>();
    for (const auto& point: *get_base_scored_points()) {
        const auto point_pos = point.position();
        base_points->emplace_back(point_pos.x(), point_pos.y(), point_pos.z());
    }
    const Kernel::Iso_cuboid_3 bound = CGAL::bounding_box(base_points->begin(), base_points->end());

    const double units_per_meter = get_units_per_centimeter() * 100;
    const double step_size_meters = 1. / static_cast<double>(points_per_meter);
    const double step_size = step_size_meters * units_per_meter;

    const Vector3d middle_point = Vector3d::Zero();
    const double distance_to_origin_units = _distance_to_origin * get_units_per_centimeter() * 1.5;
    const double begin_x = std::max(middle_point.x() - distance_to_origin_units, bound.xmin());
    const double begin_y = std::max(middle_point.y() - distance_to_origin_units, bound.ymin());
    const double begin_z = std::max(middle_point.z() - distance_to_origin_units, bound.zmin());

    const double end_x = std::min(middle_point.x() + distance_to_origin_units, bound.xmax());
    const double end_y = std::min(middle_point.y() + distance_to_origin_units, bound.ymax());
    const double end_z = std::min(middle_point.z() + distance_to_origin_units, bound.zmax());

    const double span_x = (end_x - begin_x);
    const double span_y = (end_y - begin_y);
    const double span_z = (end_z - begin_z);

    auto result = make_shared<vector<ScoredPoint>>();
    size_t point_count = ceil(
            (span_x / step_size - 1)
            * (span_y / step_size - 1)
            * (span_z / step_size - 1));

    std::clog << "A maximum of " << std::to_string(point_count)
              << " points to be generated" << std::endl;

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<double> dist(-step_size / 2, step_size / 2);

    const auto distance_threshold = distance_to_origin_units;
    size_t index = 0;
    auto x = begin_x;
    auto time = start_time();
    while (x < end_x) {
        auto y = begin_y;
        while (y < end_y) {
            auto z = begin_z;
            while (z < end_z) {
                const Vector3d point_vector(x + dist(e2), y + dist(e2), z + dist(e2));
                const double distance_from_origin = (middle_point - point_vector).norm();
                if (distance_from_origin <= distance_threshold) {
                    result->emplace_back(point_vector, index, 0);

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

double PointCloudProvider::get_units_per_centimeter() const {
    return this->sparse_cloud->get_units_per_centimeter(this->_distance_to_origin);
}
