#include "dense_cloud.h"

namespace rcsop::common {
    namespace PMP = CGAL::Polygon_mesh_processing;

    DenseCloud::DenseCloud(const path& ply_file_path) : BasePointCloud(ply_file_path) {
        _mesh = make_unique<SurfaceMesh>();
        CGAL::IO::read_polygon_mesh(this->model_path().string(), *_mesh);

        PMP::triangulate_faces(*_mesh);

        if (CGAL::is_closed(*_mesh)) {
            _inside = make_unique<InsideTriangleMesh>(*_mesh);
        } else {
            _inside = nullptr;
        }
    }

    bool DenseCloud::is_inside(const vec3& point) const {
        if (!CGAL::is_closed(*_mesh)) {
            throw std::runtime_error("Dense mesh not closed, hence cannot determine whether any point is inside.");
        }
        assert(_inside != nullptr);
        Point cgalPoint(point.x(), point.y(), point.z());
        CGAL::Bounded_side res = (*this->_inside)(cgalPoint);

        return res == CGAL::ON_BOUNDED_SIDE || res == CGAL::ON_BOUNDARY;
    }

    shared_ptr<vector<SimplePoint>> DenseCloud::get_points() const {
        auto result = make_shared<vector<SimplePoint>>();
        for (VertexDescriptor vertex_index: this->_mesh->vertices()) {
            const Point vertex = this->_mesh->point(vertex_index);
            result->emplace_back(vertex_index, vec3(vertex.x(), vertex.y(), vertex.z()));
        }
        return result;
    }

    void DenseCloud::filter_points(const function<bool(const vec3&)>& predicate_to_keep) {
        for (VertexDescriptor vertex_index: this->_mesh->vertices()) {
            const Point vertex = this->_mesh->point(vertex_index);
            if (!predicate_to_keep(vec3(vertex.x(), vertex.y(), vertex.z()))) {
                this->_mesh->remove_vertex(vertex_index);
            }
        }
    }

    void DenseCloud::add_point(const IdPoint* point,
                               const color_vec& color) {
        //TODO:
        throw runtime_error("Adding points to dense clouds not supported yet.");
    }

    void DenseCloud::write(const path& output_path) {
        //TODO:
        throw runtime_error("Saving dense clouds not supported yet.");
    }

    size_t DenseCloud::point_count() const {
        return this->_mesh->number_of_vertices();
    }

    void DenseCloud::purge_cameras(camera_id_t camera_to_keep) {
        //TODO log warning, nothing else to be done here
    }

    bool DenseCloud::is_available_at(const path& file_path) {
        return std::filesystem::is_regular_file(file_path);
    }
}