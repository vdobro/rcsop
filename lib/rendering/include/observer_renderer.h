#ifndef RCSOP_RENDERING_OBSERVER_RENDERER_H
#define RCSOP_RENDERING_OBSERVER_RENDERER_H

#include "utils/chronometer.h"

#include "observer.h"
#include "colors.h"
#include "scored_cloud.h"
#include "texture.h"
#include "rendering_options.h"
#include "sfml_renderer.h"

class ObserverRenderer {
private:
    Observer _observer;
    shared_ptr<vector<ScoredPoint>> _points;
    sfm::rendering::global_colormap_func _color_map;

    shared_ptr<SfmlRenderer> _renderer = nullptr;

    shared_ptr<vector<pair<TextureRenderParams, Texture>>> _textures;

    vector<RenderedPoint> project_points();

public:
    explicit ObserverRenderer(const ScoredCloud& pointsWithObserver,
                              const sfm::rendering::global_colormap_func& color_map,
                              const rendering_options& options);

    void add_texture(Texture texture, TextureRenderParams coordinates);

    void render(const path& output_path,
                const string& log_prefix);
};

#endif //RCSOP_RENDERING_OBSERVER_RENDERER_H
