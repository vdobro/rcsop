#ifndef RCSOP_RENDERING_OBSERVER_RENDERER_H
#define RCSOP_RENDERING_OBSERVER_RENDERER_H

#include "utils/chronometer.h"

#include "observer.h"
#include "colors.h"
#include "scored_cloud.h"
#include "texture.h"
#include "rendering_options.h"
#include "base_renderer.h"

class ObserverRenderer {
private:
    Observer _observer;
    shared_ptr<vector<ScoredPoint>> _points;
    sfm::rendering::global_colormap_func _color_map;

    shared_ptr<BaseRenderer> _renderer = nullptr;

    shared_ptr<vector<pair<texture_rendering_options, Texture>>> _textures;

    vector<rendered_point> project_points();

public:
    explicit ObserverRenderer(const ScoredCloud& pointsWithObserver,
                              const sfm::rendering::global_colormap_func& color_map,
                              const rendering_options& options);

    void add_texture(Texture texture, texture_rendering_options coordinates);

    void render(const path& output_path,
                const string& log_prefix);
};

#endif //RCSOP_RENDERING_OBSERVER_RENDERER_H
