#ifndef SFM_COLORING_OBSERVER_RENDERER_H
#define SFM_COLORING_OBSERVER_RENDERER_H

#include <SFML/Graphics/RenderTexture.hpp>

#include "observer.h"
#include "render_points.h"
using namespace sfm::rendering;

#include "utils/chronometer.h"

class ObserverRenderer {
private:
    const Observer& _observer;
    shared_ptr<sf::Shader> _shader;

    vector<RenderedPoint> project_points(const vector<scored_point>& points,
                                         const global_colormap_func& colormap);

    void render_point(const RenderedPoint& point, sf::RenderTarget& render_target);

public:
    explicit ObserverRenderer(const Observer& observer) : _observer(observer) {
        this->_shader = sfm::rendering::initialize_renderer();
    }

    void render(const path& output_path,
                const vector<scored_point>& points,
                const global_colormap_func& colormap,
                const string& log_prefix);

};


#endif //SFM_COLORING_OBSERVER_RENDERER_H
