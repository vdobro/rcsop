#include "utils.h"
#include "scored_point.h"
#include "colors.h"
#include "render_points.h"

using std::vector;

using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Vector3ub;

using colmap::Image;
using colmap::Reconstruction;

static inline double get_sign(const Vector2d& p1,
                              const Vector2d& p2,
                              const Vector2d& p3) {
    return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
}

static bool inline is_in_triangle(const Vector2d& point,
                                  const Vector2d& v1,
                                  const Vector2d& v2,
                                  const Vector2d& v3) {
    auto b1 = get_sign(point, v1, v2) < 0.0;
    auto b2 = get_sign(point, v2, v3) < 0.0;
    auto b3 = get_sign(point, v3, v1) < 0.0;
    return (b1 == b2) && (b2 == b3);
}

void color_slices(const model_ptr& model,
                  const vector<double>& rcs,
                  const string& output_path) {
    auto images = get_images(*model);
    auto image_count = images.size();
    auto image_positions = map_vec<Image, Vector3d>(images, get_image_position);

    vector<scored_point> points = map_vec<point_pair, scored_point>(get_points(*model), [](point_pair pair) {
        return scored_point(pair);
    });

    auto origin = Vector2d();
    origin.setZero();

    const auto rcs_colors = color_values(rcs, map_turbo);

    for (auto& point: points) {
        auto flat_point = point.flat_down();

        for (size_t image_id = 0; image_id < image_count; image_id++) {
            auto previous_camera_id = image_id == 0 ? image_count - 1 : (image_id - 1);
            auto previous_camera = flat_down_from_above(image_positions[previous_camera_id]);
            auto next_camera = flat_down_from_above(image_positions[(image_id + 1) % image_count]);
            auto current_camera = flat_down_from_above(image_positions[image_id]);

            auto midpoint_to_previous = (previous_camera + current_camera) / 2;
            auto midpoint_to_next = (current_camera + next_camera) / 2;

            if (is_in_triangle(flat_point, midpoint_to_previous, midpoint_to_next, origin)) {
                model->Point3D(point.id()).Color() = rcs_colors[image_id];
                point.increment_score(rcs[image_id]);
                continue;
            }
        }
    }

    write_model(model, output_path);
    for (size_t image_id = 1; image_id <= image_count; image_id++) {
        render_to_image(model, image_id, "data/Audi_40", "data/renders/audi_40", points);
    }
}
