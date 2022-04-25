#pragma once

#include <string>
#include <memory>
#include <algorithm>
#include <filesystem>

#include "colmap/base/reconstruction.h"

using std::string;
using std::unique_ptr;

unique_ptr<colmap::Reconstruction> read_model(const string &path);

void write_model(const colmap::Reconstruction& model, const string &output_path);

std::vector<colmap::Image> get_images(const colmap::Reconstruction& model);

typedef std::pair<unsigned long, colmap::Point3D> point_pair;
std::vector<point_pair> get_points(const colmap::Reconstruction& model);

Eigen::Vector3d get_image_position(const colmap::Image& image);

Eigen::Vector2d flat_down_from_above(const Eigen::Vector3d& point);

template<typename Source, typename Target, typename MapFunction>
std::vector<Target> map_vec(const std::vector<Source> &vec, MapFunction map_function) {
    std::vector<Target> result;
    result.resize(vec.size());
    std::transform(vec.begin(), vec.end(), result.begin(), map_function);
    return result;
}
