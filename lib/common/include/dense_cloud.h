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

namespace rcsop::common {
    using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point = Kernel::Point_3;
    using SurfaceMesh = CGAL::Surface_mesh<Point>;
    using InsideTriangleMesh = CGAL::Side_of_triangle_mesh<SurfaceMesh, Kernel>;
    using VertexDescriptor = SurfaceMesh::Vertex_index;

    using rcsop::common::utils::points::Vector3d;

    class DenseCloud {
    public:
        explicit DenseCloud(const path& ply_file_path);

        [[nodiscard]] bool is_inside(const Vector3d& point) const;

        [[nodiscard]] shared_ptr<vector<Vector3d>> points() const;

    private:
        shared_ptr<SurfaceMesh> mesh;
        shared_ptr<InsideTriangleMesh> inside;
    };
}
#endif //RCSOP_COMMON_DENSE_CLOUD_H
