#include "utils.h"

typedef Eigen::Hyperplane<double, 3> plane;

static Vector3d get_right(const Image& image) {
    Vector3d local_right;
    local_right.setZero();
    local_right.x() = 1;
    return transform_to_world(image, local_right).normalized();
}

static Vector3d get_up(const Image& image) {
    Vector3d local_right;
    local_right.setZero();
    local_right.y() = -1;
    return transform_to_world(image, local_right).normalized();
}

void sum_pyramids(const model_ptr& model,
                  const vector<double>& rcs,
                  const string& output_path) {
    auto images = get_images(*model);
    auto image_count = images.size();

    for (size_t i = 0; i < image_count; i++) {
        auto image = images[i];
        auto rcs_value = rcs[i];

        auto image_pos = get_image_position(image);
        auto right = get_right(image);
        auto up = get_up(image);

        plane vertical_plane = plane(image_pos, right);
        plane horizontal_plane = plane(image_pos, up);

        auto points = get_points(*model);
        for (const auto& point_pair: points) {
            auto point = point_pair.second;
            auto point_position = point.XYZ();

            auto line_to_vertical_plane = model_line (point_position, vertical_plane.normal());
            auto line_to_horizontal_plane = model_line (point_position, horizontal_plane.normal());

            auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
            auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

            auto distance_to_camera = (point_position - image_pos);
        }
    }
}