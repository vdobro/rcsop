#include "rcs_slices.h"

#include <execution>

#include "utils/logging.h"

#include "model_camera.h"
#include "observer_provider.h"
#include "point_cloud_provider.h"
#include "observer_renderer.h"
#include "output_data_writer.h"
#include "colors.h"

namespace rcsop::launcher::tasks {
    using rcsop::common::utils::points::vec2;
    using rcsop::common::utils::points::vec3;

    using rcsop::common::utils::map_vec_shared;
    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::filter_vec_shared;

    using rcsop::common::ModelCamera;
    using rcsop::common::height_t;
    using rcsop::common::Observer;
    using rcsop::common::ScoredPoint;
    using rcsop::common::IdPoint;
    using rcsop::common::ScoredCloud;
    using rcsop::common::OutputDataWriter;

    using rcsop::data::ObserverProvider;
    using rcsop::data::PointCloudProvider;
    using rcsop::data::SIMPLE_RCS_MAT;
    using rcsop::common::utils::rcs::rcs_value_t;

    using rcsop::common::coloring::construct_color_map_function;
    using rcsop::common::coloring::color_values;
    using rcsop::rendering::ObserverRenderer;

    using rcsop::launcher::utils::batch_output;

    static inline auto flat_down_from_above(const vec3& point) -> vec2 {
        auto res = vec2();
        res.x() = point.x();
        res.y() = point.z();
        return res;
    }

    static inline auto get_sign(const vec2& p1,
                                const vec2& p2,
                                const vec2& p3) -> double {
        return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
    }

    static inline auto is_in_triangle(const vec2& point,
                                      const vec2& v1,
                                      const vec2& v2,
                                      const vec2& v3) -> bool {
        auto b1 = get_sign(point, v1, v2) < 0.0;
        auto b2 = get_sign(point, v2, v3) < 0.0;
        auto b3 = get_sign(point, v3, v1) < 0.0;
        return (b1 == b2) && (b2 == b3);
    }

    static inline auto is_within_camera_slice(
            const vec2& point,
            const vec2& origin,
            const size_t& image_id,
            const size_t& image_count,
            const vector<vec2>& image_positions) -> bool {
        auto previous_camera_id = image_id == 0 ? image_count - 1 : (image_id - 1);
        const auto& previous_camera = image_positions[previous_camera_id];
        const auto& next_camera = image_positions[(image_id + 1) % image_count];
        const auto& current_camera = image_positions[image_id];

        auto midpoint_to_previous = (previous_camera + current_camera) / 2;
        auto midpoint_to_next = (current_camera + next_camera) / 2;

        return is_in_triangle(point, midpoint_to_previous, midpoint_to_next, origin);
    }

    void rcs_slices(const InputDataCollector& inputs,
                    const task_options& options) {
        const auto observer_provider = make_shared<ObserverProvider>(inputs, options.camera, false);
        const auto point_provider = make_shared<PointCloudProvider>(inputs, options.camera);

        auto observers = observer_provider->observers_with_positions();
        const auto cameras = map_vec<Observer, ModelCamera>(observers, &Observer::native_camera);
        const auto image_positions = map_vec<ModelCamera, vec3>(cameras, &ModelCamera::position);
        const auto flattened_image_positions = map_vec<vec3, vec2>(image_positions, flat_down_from_above);
        const auto image_count = image_positions.size();

        const size_t take_every_nth = options.rendering.use_gpu_rendering ? 20 : 15;
        const auto base_points = point_provider->get_base_points(take_every_nth);

        const auto origin = vec2(0, 0);
        const height_t default_height = options.camera.default_height;
        const auto data = inputs.data<SIMPLE_RCS_MAT>()
                ->at_height(default_height)
                ->rcs();
        const vector<height_t> heights{default_height};

        auto points = map_vec_shared<IdPoint, ScoredPoint, true>(
                *base_points,
                [&data, &origin, &flattened_image_positions, &image_count]
                        (const IdPoint& point) {
                    auto flat_point = flat_down_from_above(point.position());

                    for (size_t image_id = 0; image_id < image_count; image_id++) {
                        if (is_within_camera_slice(flat_point, origin, image_id, image_count,
                                                   flattened_image_positions)) {
                            rcs_value_t value = data[image_id];
                            return ScoredPoint(point.position(), point.id(), value);
                        }
                    }
                    return ScoredPoint(point.position(), point.id());
                });
        auto filtered_points = filter_vec_shared<ScoredPoint>(
                *points, [](const ScoredPoint& point) {
                    return !point.is_discarded();
                });

        auto score_range = ScoredPoint::get_score_range(*filtered_points);
        auto color_map = construct_color_map_function(options.rendering.color_map, score_range);

        auto renderers = map_vec<Observer, shared_ptr<OutputDataWriter>>(
                observers,
                [&filtered_points, &color_map, &options](const Observer& observer) -> shared_ptr<OutputDataWriter> {
                    ScoredCloud payload(observer, filtered_points);
                    return make_shared<ObserverRenderer>(payload, color_map, options.rendering);
                });

        batch_output(renderers, options, heights);
    }
}
