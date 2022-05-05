#include "cleanup.h"

static double distance_from_origin(const Vector3d& point) {
    return point.norm();
}

static double min_distance(const vector<Image>& images) {
    auto image_positions = map_vec<Image, Vector3d>(images, get_image_position);
    auto distances = map_vec<Vector3d, double>(image_positions, distance_from_origin);
    return *std::min_element(distances.begin(), distances.end());
}

void filter_points(const shared_ptr<Reconstruction>& model,
                   const string& output_path) {
    auto points = model->Points3D();
    vector<Image> images = get_images(*model);

    auto max_camera_distance = min_distance(images);

    for (const auto& point_with_id: points) {
        auto point_id = point_with_id.first;
        auto point = point_with_id.second;
        if (distance_from_origin(point.XYZ()) > max_camera_distance * 0.9) {
            model->DeletePoint3D(point_id);
        }
    }

    write_model(model, output_path);
}