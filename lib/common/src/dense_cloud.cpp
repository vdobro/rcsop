#include "dense_cloud.h"

namespace PMP = CGAL::Polygon_mesh_processing;

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3                                     Point;
typedef CGAL::Surface_mesh<Point>                           SurfaceMesh;
typedef CGAL::Side_of_triangle_mesh<SurfaceMesh, Kernel>    InsideTriangleMesh;
typedef SurfaceMesh::Vertex_index                           VertexDescriptor;

DenseCloud::DenseCloud(const path& ply_file_path) {
    mesh = make_shared<SurfaceMesh>();
    CGAL::IO::read_polygon_mesh(ply_file_path.string(), *mesh);

    PMP::triangulate_faces(*mesh);

    inside = make_shared<InsideTriangleMesh>(*mesh);
}

bool DenseCloud::is_inside(const Eigen::Vector3d& point) {
    Point cgalPoint(point.x(), point.y(), point.z());
    CGAL::Bounded_side res = (*this->inside)(cgalPoint);

    return res == CGAL::ON_BOUNDED_SIDE || res == CGAL::ON_BOUNDARY;
}

vector<Vector3d> DenseCloud::points() {
    vector<Vector3d> result;
    for (VertexDescriptor vertex_index : this->mesh->vertices()) {
        const Point vertex = this->mesh->point(vertex_index);
        Vector3d point(vertex.x(), vertex.y(), vertex.z());
        result.push_back(point);
    }
    return result;
}
