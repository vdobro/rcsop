#include "rcs_slices.h"

#include <execution>

#include "colors.h"
#include "options.h"
#include "../default_options.h"

#include "observer_provider.h"
#include "point_cloud_provider.h"
#include "observer_renderer.h"
#include "utils/logging.h"

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

static inline bool is_in_triangle(const Vector2d& point,
                                  const Vector2d& v1,
                                  const Vector2d& v2,
                                  const Vector2d& v3) {
    auto b1 = get_sign(point, v1, v2) < 0.0;
    auto b2 = get_sign(point, v2, v3) < 0.0;
    auto b3 = get_sign(point, v3, v1) < 0.0;
    return (b1 == b2) && (b2 == b3);
}

static inline bool is_within_camera_slice(
        const Vector2d& point,
        const Vector2d& origin,
        const size_t& image_id,
        const size_t& image_count,
        const vector<Vector2d>& image_positions) {
    auto previous_camera_id = image_id == 0 ? image_count - 1 : (image_id - 1);
    auto previous_camera = image_positions[previous_camera_id];
    auto next_camera = image_positions[(image_id + 1) % image_count];
    auto current_camera = image_positions[image_id];

    auto midpoint_to_previous = (previous_camera + current_camera) / 2;
    auto midpoint_to_next = (current_camera + next_camera) / 2;

    return is_in_triangle(point, midpoint_to_previous, midpoint_to_next, origin);
}

void rcs_slices(const shared_ptr<InputDataCollector>& inputs,
                const path& output_path) {
    const auto observer_provider = make_shared<ObserverProvider>(*inputs, CAMERA_DISTANCE);
    const auto point_provider = make_shared<PointCloudProvider>(*inputs);

    auto observers = observer_provider->observers();
    const auto cameras = map_vec<Observer, camera, false>(observers, &Observer::native_camera);
    const auto image_positions = map_vec<camera, Vector3d>(cameras, &camera::position);
    const auto flattened_image_positions = map_vec<Vector3d, Vector2d>(image_positions, flat_down_from_above);
    const auto image_count = image_positions.size();

    const auto base_points = point_provider->get_base_scored_points();

    const auto origin = Vector2d(0, 0);
    const auto data = inputs->data<SIMPLE_RCS_MAT>(false)
            ->at_height(DEFAULT_HEIGHT)
            ->rcs();
    const auto rcs_colors = color_values(data, map_turbo);

    const auto points = map_vec_shared<ScoredPoint, ScoredPoint>(
            *base_points,
            [&data, &origin, &flattened_image_positions, &image_count]
                    (const ScoredPoint& point) {
                auto flat_point = flat_down_from_above(point.position());

                for (size_t image_id = 0; image_id < image_count; image_id++) {
                    if (is_within_camera_slice(flat_point, origin, image_id, image_count, flattened_image_positions)) {
                        return ScoredPoint(point.position(), point.id(), data[image_id]);
                    }
                }
                return ScoredPoint(point.position(), point.id());
            });

    auto renderers = map_vec<Observer, ObserverRenderer, false>(
            observers, [&points](const Observer& observer) -> ObserverRenderer {
                ScoredCloud payload(observer, points);
                return ObserverRenderer(payload);
            });

    auto score_range = ScoredPoint::get_score_range(*points);
    auto colormap = construct_colormap_function(COLOR_MAP, score_range);

    size_t index = 0;
    size_t last = renderers.size();
    for (auto& renderer: renderers) {
        renderer.render(output_path, colormap, construct_log_prefix(index++, last));
    }
}
