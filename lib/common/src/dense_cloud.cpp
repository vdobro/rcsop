#include "dense_cloud.h"

namespace rcsop::common {
    namespace PMP = CGAL::Polygon_mesh_processing;

    DenseCloud::DenseCloud(const path& ply_file_path) {
        mesh = make_shared<SurfaceMesh>();
        CGAL::IO::read_polygon_mesh(ply_file_path.string(), *mesh);

        PMP::triangulate_faces(*mesh);

        inside = make_shared<InsideTriangleMesh>(*mesh);
    }

    bool DenseCloud::is_inside(const vec3& point) const {
        Point cgalPoint(point.x(), point.y(), point.z());
        CGAL::Bounded_side res = (*this->inside)(cgalPoint);

        return res == CGAL::ON_BOUNDED_SIDE || res == CGAL::ON_BOUNDARY;
    }

    shared_ptr<vector<vec3>> DenseCloud::points() const {
        auto result = make_shared<vector<vec3>>();
        for (VertexDescriptor vertex_index: this->mesh->vertices()) {
            const Point vertex = this->mesh->point(vertex_index);
            result->emplace_back(vertex.x(), vertex.y(), vertex.z());
        }
        return result;
    }
}