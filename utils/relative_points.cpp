#include "relative_points.h"

typedef Eigen::Hyperplane<double, 3> plane;

static Vector3d get_right(const Image& image) {
    Vector3d local_right;
    local_right.setZero();
    local_right.x() = 1;
    auto direction = transform_to_world(image, local_right) - get_image_position(image);
    return direction.normalized();
}

static Vector3d get_up(const Image& image) {
    Vector3d local_up;
    local_up.setZero();
    local_up.y() = -1;
    auto direction = transform_to_world(image, local_up) - get_image_position(image);
    return direction.normalized();
}

#define ARC_SIN(X) (asin(X) * 180.f / M_PI)

vector<relative_point> get_point_angles(const Image& image,
                                        double height_offset,
                                        const scored_point_map& points) {
    auto right = get_right(image);
    auto up = get_up(image);
    Vector3d image_pos = get_image_position(image) + (height_offset * up);

    const plane vertical_plane = plane(right, image_pos);
    const plane horizontal_plane = plane(up, image_pos);

    vector<relative_point> result;
    for (const auto& point_pair: points) {
        auto id = point_pair.first;
        auto point = point_pair.second;
        Vector3d point_position = point.position();

        auto distance_to_vertical_plane = vertical_plane.absDistance(point_position);
        auto distance_to_horizontal_plane = horizontal_plane.absDistance(point_position);

        auto distance_to_camera = (point_position - image_pos).norm();

        auto is_to_the_right = (point_position - image_pos).dot(right) > 0;
        auto is_up_above = (point_position - image_pos).dot(up) > 0;

        auto horizontal_angle = (is_to_the_right ? 1 : -1) * ARC_SIN(distance_to_vertical_plane / distance_to_camera);
        auto vertical_angle = (is_up_above ? 1 : -1) * ARC_SIN(distance_to_horizontal_plane / distance_to_camera);

        relative_point point_info = {
                .position = point.position(),
                .id = id,
                .distance = distance_to_camera,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
        result.push_back(point_info);
    }
    return result;
}

double find_interval_match(double search_value,
                           const vector<double>& mapped_values,
                           const double first_range,
                           const double range_epsilon) {
    auto index = lround((search_value - first_range) / (2 * range_epsilon));
    return mapped_values[index];
}
