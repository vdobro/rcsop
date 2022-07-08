#ifndef RCSOP_RENDERING_SFML_RENDERER_CONTEXT_H
#define RCSOP_RENDERING_SFML_RENDERER_CONTEXT_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "base_renderer.h"
#include "rendering_options.h"

class SfmlRendererContext : public BaseRendererContext {
private:
    const gradient_options& _options;

    shared_ptr<sf::Shader> _shader;
    unique_ptr<sf::RenderTexture> _render_target;

public:
    explicit SfmlRendererContext(const Observer& observer,
                                 shared_ptr<sf::Shader> shader,
                                 const gradient_options& options);

    void render_point(const rendered_point& point) override;

    void render_texture(const Texture& texture,
                        const texture_rendering_options& options) override;

    void write_to_image(const path& output_path) override;
};

#endif //RCSOP_RENDERING_SFML_RENDERER_CONTEXT_H
