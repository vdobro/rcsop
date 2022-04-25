#include "utils.h"
#include <algorithm>
#include <filesystem>

using std::make_shared;
using std::sort;
using std::vector;

using colmap::Image;
using colmap::Point3D;
using colmap::Reconstruction;
using Eigen::Vector2d;
using Eigen::Vector3d;

unique_ptr<Reconstruction> read_model(const string &path) {
    auto model = std::make_unique<Reconstruction>();
    model->Read(path);

    return model;
}

void write_model(const colmap::Reconstruction &model, const string &output_path) {
    std::filesystem::remove_all(output_path);
    std::filesystem::create_directory(output_path);
    model.Write(output_path);
}

std::vector<Image> get_images(const colmap::Reconstruction &model) {
    std::vector<Image> images;
    auto image_count = model.NumImages();
    images.resize(image_count);
    auto image_map = model.Images();

    for (size_t i = 1; i <= image_count; ++i) {
        auto image = image_map[i];
        images[i - 1] = image;
    }
    return images;
}

vector<point_pair> get_points(const Reconstruction &model) {
    auto point_map = model.Points3D();

    vector<point_pair> point_pairs(point_map.begin(), point_map.end());

    sort(point_pairs.begin(), point_pairs.end(), [](const point_pair &a, const point_pair &b) {
        return a.first > b.first;
    });

    return point_pairs;
}

static Vector3d transform_to_world(const Image &image, const Vector3d &image_xyz) {
    return image.InverseProjectionMatrix() * image_xyz.homogeneous();
}

Vector3d get_image_position(const Image &image) {
    Vector3d origin = Vector3d();
    origin.setZero();
    return transform_to_world(image, origin);
}