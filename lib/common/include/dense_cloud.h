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

class DenseCloud {
public:
    explicit DenseCloud(const path& ply_file_path);

    bool is_inside(const Vector3d& point);

    vector<Vector3d> points();

private:
    typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
    typedef Kernel::Point_3                                     Point;
    typedef CGAL::Surface_mesh<Point>                           SurfaceMesh;
    typedef CGAL::Side_of_triangle_mesh<SurfaceMesh, Kernel>    InsideTriangleMesh;

    shared_ptr<SurfaceMesh> mesh;
    shared_ptr<InsideTriangleMesh> inside;
};

#endif //RCSOP_COMMON_DENSE_CLOUD_H
