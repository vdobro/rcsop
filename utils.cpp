#include "utils.h"

#include <algorithm>
#include <filesystem>

using std::make_shared;
using std::sort;
using std::vector;

using std::filesystem::create_directories;
using std::filesystem::remove_all;

using colmap::Image;
using colmap::Point3D;
using colmap::Reconstruction;
using Eigen::Vector2d;
using Eigen::Vector3d;

shared_ptr<Reconstruction> read_model(const string& path) {
    auto model = make_shared<Reconstruction>();
    model->Read(path);

    return model;
}

void write_model(const model_ptr& model, const string& output_path) {
    remove_all(output_path);
    create_directories(output_path);

    model->Write(output_path);
}

vector<Image> get_images(const Reconstruction& model) {
    vector<Image> images;
    auto image_count = model.NumImages();
    images.resize(image_count);
    auto image_map = model.Images();

    for (size_t i = 1; i <= image_count; ++i) {
        auto image = image_map[i];
        images[i - 1] = image;
    }
    return images;
}

vector<point_pair> get_points(const Reconstruction& model) {
    auto point_map = model.Points3D();

    vector<point_pair> point_pairs(point_map.begin(), point_map.end());

    sort(point_pairs.begin(), point_pairs.end(), [](const point_pair& a, const point_pair& b) {
        return a.first > b.first;
    });

    return point_pairs;
}

static Vector3d transform_to_world(const Image& image,
                                   const Vector3d& image_xyz) {
    return image.InverseProjectionMatrix() * image_xyz.homogeneous();
}

Vector3d get_image_position(const Image& image) {
    Vector3d origin = Vector3d();
    origin.setZero();
    return transform_to_world(image, origin);
}

Vector2d flat_down_from_above(const Vector3d& point) {
    auto res = Vector2d();
    res.x() = point.x();
    res.y() = point.z();
    return res;
}
