#ifndef RCSOP_RENDERING_OBSERVER_RENDERER_H
#define RCSOP_RENDERING_OBSERVER_RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "observer.h"
#include "utils/chronometer.h"
#include "colors.h"
#include "scored_cloud.h"
#include "texture.h"

struct RenderedPoint {
    Vector2d coordinates = Vector2d::Zero();
    Vector3ub color = Vector3ub::Zero();
};

struct TextureRenderParams {
    Vector2d coordinates = Vector2d::Zero();
    Vector2d size = Vector2d::Zero();
};

class ObserverRenderer {
private:
    const Observer _observer;
    const shared_ptr<vector<ScoredPoint>> _points;
    shared_ptr<vector<pair<TextureRenderParams, Texture>>> _textures;

    shared_ptr<sf::Shader> _shader;

    vector<RenderedPoint> project_points(const sfm::rendering::global_colormap_func& colormap);

    void render_point(const RenderedPoint& point, sf::RenderTarget& render_target);

    void initialize_renderer();

public:
    explicit ObserverRenderer(const ScoredCloud& pointsWithObserver);

    void add_texture(Texture texture, TextureRenderParams coordinates);

    void render(const path& output_path,
                const sfm::rendering::global_colormap_func& colormap,
                const string& log_prefix);

};

#endif //RCSOP_RENDERING_OBSERVER_RENDERER_H
