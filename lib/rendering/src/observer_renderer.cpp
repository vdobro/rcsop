#include "observer_renderer.h"
#include "sfml_renderer.h"
#include "cairo_renderer.h"

#include <utility>

namespace rcsop::rendering {
    using std::isnan;
    using std::cerr;

    using rcsop::common::utils::time::start_time;
    using rcsop::common::utils::time::log_and_start_next;
    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::sort_in_place;

    using rcsop::common::ModelCamera;

    using rcsop::rendering::rendered_point;

    auto ObserverRenderer::get_point_perspective_scale(const ImagePoint& point) const -> double {
        return atan(_reference_radius / _reference_distance)
               / atan(point.distance() / _reference_distance);
    }

    vector<rendered_point> ObserverRenderer::project_in_camera_with_color(
            const vector<ImagePoint>& points,
            const ModelCamera& camera,
            const global_colormap_func& color_map) const {
        return map_vec<ImagePoint, rendered_point>(points, [this, camera, color_map](const ImagePoint& point) {
            auto camera_coordinates = camera.project_from_image(point.coordinates());
            auto score = point.score();
            if (isnan(score)) {
                throw invalid_argument("Score of point is not a number");
            }
            auto color = color_map(point.score());
            return rendered_point{
                    .coordinates = camera_coordinates,
                    .size_factor = static_cast<float>(get_point_perspective_scale(point)),
                    .color = color
            };
        });
    }

    vector<rendered_point> ObserverRenderer::project_points() const {
        const auto& camera = _observer.native_camera();
        auto img_points = camera.project_to_image(*_points);

        sort_in_place<ImagePoint, double>(img_points, [](const ImagePoint& point) {
            return point.distance();
        });
        auto rendered_points = project_in_camera_with_color(
                img_points, camera, this->_color_map);
        return rendered_points;
    }

    void ObserverRenderer::write(const path& output_path,
                                 const string& log_prefix) {
        auto time_measure = start_time();

        const auto& camera = _observer.native_camera();
        const path input_file_path = _observer.source_image_path();
        const string file_name = "data-" + _observer.position().str() + "__source-" + camera.get_last_name_segment();
        const path output_file_path{output_path / file_name};
        string output_name = output_file_path.filename().string();

        shared_ptr<BaseRendererContext> renderer_context = this->_renderer->create_context(_observer);

        vector<rendered_point> rendered_points = this->project_points();

        for (const auto& point: rendered_points) {
            renderer_context->render_point(point);
        }
        for (const auto& [rendering_options, texture]: *_textures) {
            renderer_context->render_texture(texture, rendering_options);
        }

        create_directories(output_file_path.parent_path());
        renderer_context->write_to_image(output_file_path);

        log_and_start_next(time_measure, log_prefix + "\tRendered image " + output_name
                                         + " with " + std::to_string(rendered_points.size()) + " points");
    }

    ObserverRenderer::ObserverRenderer(
            const ScoredCloud& points_with_observer,
            const global_colormap_func& color_map,
            const rendering_options& options,
            double reference_distance_centimeters)
            : _observer(points_with_observer.observer()),
              _points(points_with_observer.points()),
              _color_map(color_map) {
        this->_textures = make_unique<vector<pair<texture_rendering_options, Texture>>>();

        this->_reference_radius = options.gradient.radius;
        this->_reference_distance = _observer.world_to_local_units(reference_distance_centimeters);

        if (options.use_gpu_rendering) {
            this->_renderer = make_unique<SfmlRenderer>(options.gradient);
        } else {
            this->_renderer = make_unique<CairoRenderer>(options.gradient);
        }
    }

    void ObserverRenderer::add_texture(Texture texture,
                                       texture_rendering_options coordinates) {
        this->_textures->emplace_back(coordinates, texture);
    }

    bool ObserverRenderer::observer_has_position() const {
        return this->_observer.has_position();
    }

    height_t ObserverRenderer::observer_height() const {
        assert(this->observer_has_position());

        return this->_observer.position().height;
    }

    string ObserverRenderer::path_prefix() const {
        return "images";
    }
}
