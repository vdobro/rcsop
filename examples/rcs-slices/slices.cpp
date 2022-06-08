#include "slices.h"

#include "colors.h"
#include "render_points.h"

#include "options.h"

using namespace sfm::rendering;

static inline Vector2d flat_down_from_above(const Vector3d& point) {
    auto res = Vector2d();
    res.x() = point.x();
    res.y() = point.z();
    return res;
}

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

void color_slices(SparseCloud& model,
                  const vector<double>& rcs,
                  const path& input_path,
                  const path& output_path) {
    auto images = model.get_cameras();
    auto image_count = images.size();
    auto image_positions = model.get_camera_positions();

    vector<scored_point> points = map_vec<point_pair, scored_point>(model.get_point_pairs(), [](const point_pair& pair) {
        return scored_point(pair);
    });

    auto origin = Vector2d();
    origin.setZero();

    const auto rcs_colors = color_values(rcs, map_turbo);

    for (auto& point: points) {
        auto flat_point = flat_down_from_above(point.position());

        for (size_t image_id = 0; image_id < image_count; image_id++) {
            auto previous_camera_id = image_id == 0 ? image_count - 1 : (image_id - 1);
            auto previous_camera = flat_down_from_above(image_positions[previous_camera_id]);
            auto next_camera = flat_down_from_above(image_positions[(image_id + 1) % image_count]);
            auto current_camera = flat_down_from_above(image_positions[image_id]);

            auto midpoint_to_previous = (previous_camera + current_camera) / 2;
            auto midpoint_to_next = (current_camera + next_camera) / 2;

            if (is_in_triangle(flat_point, midpoint_to_previous, midpoint_to_next, origin)) {
                model.set_point_color(point.id(), rcs_colors[image_id]);
                point.increment_score(rcs[image_id]);
                continue;
            }
        }
    }

    std::filesystem::create_directories(output_path);
    model.save(output_path);

    const path render_path{output_path / "render"};
    std::filesystem::remove_all(render_path);

    auto colormap = get_colormap(points, COLOR_MAP);
    render_images(model, input_path, render_path, points, colormap);
}
