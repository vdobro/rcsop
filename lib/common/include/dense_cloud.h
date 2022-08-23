#ifndef RCSOP_COMMON_DENSE_CLOUD_H
#define RCSOP_COMMON_DENSE_CLOUD_H

#include <filesystem>
#include <memory>
#include <vector>

#include <CGAL/Surface_mesh/Surface_mesh.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Side_of_triangle_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>

#include "utils/types.h"
#include "utils/points.h"
#include "id_point.h"
#include "base_point_cloud.h"

namespace rcsop::common {
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point = Kernel::Point_3;
    using SurfaceMesh = CGAL::Surface_mesh<Point>;
    using InsideTriangleMesh = CGAL::Side_of_triangle_mesh<SurfaceMesh, Kernel>;
    using VertexDescriptor = SurfaceMesh::Vertex_index;

    using rcsop::common::utils::points::vec3;
    using rcsop::common::IdPoint;
    using rcsop::common::BasePointCloud;

    class DenseCloud : public BasePointCloud {
    private:
        unique_ptr<SurfaceMesh> _mesh;
        unique_ptr<InsideTriangleMesh> _inside;

    public:
        explicit DenseCloud(const path& ply_file_path);

        [[nodiscard]] bool is_inside(const vec3& point) const;

        [[nodiscard]] shared_ptr<vector<IdPoint>> get_points() const override;

        [[nodiscard]] size_t point_count() const override;

        void filter_points(const function<bool(const vec3&)>& predicate_to_keep) override;

        void add_point(const vec3& point, const color_vec& color) override;

        void purge_cameras(camera_id_t camera_to_keep) override;

        void write(const path& output_path) override;
    };
}
#endif //RCSOP_COMMON_DENSE_CLOUD_H
