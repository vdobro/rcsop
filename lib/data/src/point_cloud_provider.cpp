#include "point_cloud_provider.h"

#include <fstream>

#include "utils/mapping.h"
#include "utils/chronometer.h"

using std::make_pair;

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/bounding_box.h>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_3;
using SurfaceMesh = CGAL::Surface_mesh<Point>;

PointCloudProvider::PointCloudProvider(const InputDataCollector& input) {
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

shared_ptr<vector<ScoredPoint>> PointCloudProvider::generate_homogenous_cloud() const {
    auto base_points = make_shared<vector<Point>>();
    for (const auto& point: *get_base_scored_points()) {
        const auto point_pos = point.position();
        base_points->emplace_back(point_pos.x(), point_pos.y(), point_pos.z());
    }
    const Kernel::Iso_cuboid_3 bound = CGAL::bounding_box(base_points->begin(), base_points->end());

    const double STEP_SIZE = 0.02;
    const double begin_x = bound.xmin();
    const double begin_y = bound.ymin();
    const double begin_z = bound.zmin();

    const double end_x = bound.xmax();
    const double end_y = bound.ymax();
    const double end_z = bound.zmax();

    const double span_x = (end_x - begin_x);
    const double span_y = (end_y - begin_y);
    const double span_z = (end_z - begin_z);

    auto result = make_shared<vector<ScoredPoint>>();
    size_t point_count = ceil(
            (span_x / STEP_SIZE - 1)
            * (span_y / STEP_SIZE - 1)
            * (span_z / STEP_SIZE - 1));

    auto time = start_time();
    log_and_start_next(time, "A maximum of " + std::to_string(point_count) + " points to be generated");

    const Vector3d middle_point((begin_x + end_x) / 2.,
                                (begin_y + end_y) / 2.,
                                (begin_z + end_z) / 2.);
    size_t index = 0;
    auto x = begin_x;
    while (x < end_x) {
        auto y = begin_y;
        while (y < end_y) {
            auto z = begin_z;
            while (z < end_z) {
                const Vector3d point_vector(x, y, z);
                const double distance_from_origin = (middle_point - point_vector).norm();
                if (distance_from_origin <= (span_x / 2)) {
                    result->emplace_back(point_vector, index, 0);

                    index++;
                }
                z += STEP_SIZE;
            }
            y += STEP_SIZE;
        }
        x += STEP_SIZE;
    }

    log_and_start_next(time, "Generated " + std::to_string(index + 1) + " point cloud");
    return result;
}

double PointCloudProvider::get_world_scale(double distance_to_origin) const {
    return this->sparse_cloud->get_world_scale(distance_to_origin);
}
