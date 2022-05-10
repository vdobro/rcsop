#include "utils.h"

using std::make_shared;
using std::sort;
using std::vector;

using std::filesystem::create_directories;
using std::filesystem::remove_all;

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

Vector3d transform_to_world(const Image& image, const Vector3d& image_xyz) {
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

scored_point_map get_scored_points(const Reconstruction& model) {
    auto model_points = get_points(model);
    map<point_id_t, scored_point> result;
    for (const auto& point_pair: model_points) {
        auto point_id = point_pair.first;
        auto point = point_pair.second;
        result.insert(pair<point_id_t, scored_point>(point_id, scored_point(point.XYZ(), point_id)));
    }
    return result;
}

double get_world_scale(double camera_distance_to_origin, const Reconstruction& model) {
    auto images = get_images(model);
    auto image_positions = map_vec<Image, Vector3d>(images, get_image_position);
    auto image_count = images.size();
    vector<double> distances;
    distances.resize(image_count);
    for (size_t i = 0; i < image_count; ++i) {
        auto opposing_index = (i + (image_count / 2)) % image_count;

        auto current_camera = image_positions[i];
        auto opposing_camera = image_positions[opposing_index];
        auto distance = (current_camera - opposing_camera).norm();
        distances[i] = distance;
    }
    auto average = std::reduce(distances.begin(), distances.end()) / static_cast<double>(image_count);
    return (average / 2) / camera_distance_to_origin;
}
