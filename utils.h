#pragma once

#include "colmap/base/reconstruction.h"
#include "types.h"

using colmap::Reconstruction;
using colmap::Image;
using colmap::Point3D;

using std::pair;
typedef pair<unsigned long, Point3D> point_pair;

shared_ptr<Reconstruction> read_model(const string& path);

void write_model(const model_ptr& model, const string& output_path);

vector<Image> get_images(const Reconstruction& model);

vector<point_pair> get_points(const Reconstruction& model);

Vector3d transform_to_world(const Image& image, const Vector3d& image_xyz);

Vector3d get_image_position(const Image& image);

Vector2d flat_down_from_above(const Vector3d& point);

template<typename Source, typename Target, typename MapFunction>
vector<Target> map_vec(const vector<Source>& vec,
                       MapFunction map_function) {
    vector<Target> result;
    result.resize(vec.size());
    std::transform(vec.begin(), vec.end(), result.begin(), map_function);
    return result;
}
