#ifndef RCSOP_RENDERING_OBSERVER_RENDERER_H
#define RCSOP_RENDERING_OBSERVER_RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "observer.h"
#include "utils/chronometer.h"
#include "colors.h"
#include "scored_cloud.h"

struct RenderedPoint {
    Vector2d coordinates = Vector2d::Zero();
    Vector3ub color = Vector3ub::Zero();
};

class ObserverRenderer {
private:
    const Observer _observer;
    const vector<ScoredPoint> _points;

    shared_ptr<sf::Shader> _shader;

    vector<RenderedPoint> project_points(const vector<ScoredPoint>& points,
                                         const sfm::rendering::global_colormap_func& colormap);

    void render_point(const RenderedPoint& point, sf::RenderTarget& render_target);

    void initialize_renderer();

public:
    explicit ObserverRenderer(const ScoredCloud& pointsWithObserver);

    void render(const path& output_path,
                const sfm::rendering::global_colormap_func& colormap,
                const string& log_prefix);

};

#endif //RCSOP_RENDERING_OBSERVER_RENDERER_H
