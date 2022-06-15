#include "relative_points.h"

typedef Eigen::Hyperplane<double, 3> plane;

static Vector3d get_right(const camera& camera) {
    Vector3d camera_right;
    camera_right.setZero();
    camera_right.x() = 1;
    Vector3d direction = camera.transform_to_world(camera_right) - camera.position();
    return direction.normalized();
}

static Vector3d get_up(const camera& camera) {
    Vector3d camera_up;
    camera_up.setZero();
    camera_up.y() = -1;
    Vector3d direction = camera.transform_to_world(camera_up) - camera.position();
    return direction.normalized();
}

#define ARC_SIN(X) (asin(X) * 180.f / M_PI)

//FIXME find usages of deprecated method and fix them
vector<observed_point> get_point_angles(const camera& image,
                                        double height_offset,
                                        const scored_point_map& points) {
    auto right = get_right(image);
    auto up = get_up(image);
    Vector3d image_pos = image.position() + (height_offset * up);

    const plane vertical_plane = plane(right, image_pos);
    const plane horizontal_plane = plane(up, image_pos);

    vector<observed_point> result;
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

        observed_point point_info = {
                .position = point.position(),
                .id = id,
                .distance_in_world = distance_to_camera,
                .vertical_angle = vertical_angle,
                .horizontal_angle = horizontal_angle,
        };
        result.push_back(point_info);
    }
    return result;
}
