#include "sfml_renderer.h"

#include "sfml_renderer_context.h"

namespace rcsop::rendering {

    static const char* FRAGMENT_SHADER =
            "uniform vec2 screen_res; "
            "uniform vec2 p_center; "
            "uniform vec4 p_color; "
            "uniform float p_radius; "
            "uniform float degree; "
            "float dist(vec2 a, vec2 b) { return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); } "
            "void main() {"
            "vec2 center_coords = vec2(p_center.x, screen_res.y - p_center.y); "
            "vec2 frag_coords = vec2(gl_FragCoord.x - 0.5, gl_FragCoord.y - 0.5); "
            "float d = min(1.0, dist(frag_coords, center_coords) / p_radius); "
            "vec4 center_color = p_color; "
            "vec4 outside_color = vec4(center_color.rgb, 0.0); "
            "float gradient = pow(d, 1.0/degree); "
            "gl_FragColor = mix(center_color, outside_color, gradient); "
            "}";

    SfmlRenderer::SfmlRenderer(const gradient_options& options)
            : _options(options) {
        if (!sf::Shader::isAvailable()) {
            throw runtime_error("Shaders unsupported");
        }
        this->_shader = make_shared<sf::Shader>();
        if (!this->_shader->loadFromMemory(FRAGMENT_SHADER, sf::Shader::Fragment)) {
            throw runtime_error("Could not load required shader.");
        }
    }

    shared_ptr<BaseRendererContext> SfmlRenderer::create_context(const Observer& observer) const {
        return make_shared<SfmlRendererContext>(observer, this->_shader, _options);
    }
}
