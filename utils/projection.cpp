#include "projection.h"

Vector3d transform_to_world(const Image& image, const Vector3d& image_xyz) {
    return image.InverseProjectionMatrix() * image_xyz.homogeneous();
}

Vector3d get_image_position(const Image& image) {
    Vector3d origin = Vector3d();
    origin.setZero();
    return transform_to_world(image, origin);
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