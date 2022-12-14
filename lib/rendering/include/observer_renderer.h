#ifndef RCSOP_RENDERING_OBSERVER_RENDERER_H
#define RCSOP_RENDERING_OBSERVER_RENDERER_H

#include "utils/chronometer.h"

#include "observer.h"
#include "colors.h"
#include "scored_cloud.h"
#include "texture.h"
#include "rendering_options.h"
#include "base_renderer.h"
#include "output_data_writer.h"
#include "image_point.h"

namespace rcsop::rendering {
    using rcsop::common::ScoredCloud;
    using rcsop::common::ScoredPoint;
    using rcsop::common::Texture;
    using rcsop::common::height_t;
    using rcsop::common::OutputDataWriter;
    using rcsop::common::ModelCamera;
    using rcsop::common::ImagePoint;

    using rcsop::common::coloring::global_colormap_func;

    class ObserverRenderer : public OutputDataWriter {
    private:
        Observer _observer;

        double _reference_radius;
        double _reference_distance;

        shared_ptr<vector<ScoredPoint>> _points;
        global_colormap_func _color_map;

        shared_ptr<BaseRenderer> _renderer = nullptr;

        shared_ptr<vector<pair<texture_rendering_options, Texture>>> _textures;

        [[nodiscard]] vector<rendered_point> project_points() const;

        [[nodiscard]] double get_point_perspective_scale(const common::ImagePoint& point) const;

        [[nodiscard]] vector<rendered_point> project_in_camera_with_color(
                const vector<ImagePoint>& points,
                const ModelCamera& camera,
                const global_colormap_func& color_map) const;

    public:
        ObserverRenderer(const ScoredCloud& pointsWithObserver,
                         const global_colormap_func& color_map,
                         const rendering_options& options,
                         double reference_distance_centimeters);

        [[nodiscard]] height_t observer_height() const override;

        void write(const path& output_path, const string& log_prefix) override;

        [[nodiscard]] string path_prefix() const override;

        [[nodiscard]] bool observer_has_position() const override;

        void add_texture(Texture texture, texture_rendering_options coordinates);

    };
}

#endif //RCSOP_RENDERING_OBSERVER_RENDERER_H
