#ifndef RCSOP_RENDERING_SFML_RENDERER_H
#define RCSOP_RENDERING_SFML_RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "utils/types.h"

#include "base_renderer.h"
#include "observer.h"
#include "texture.h"
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

    void render_point(const RenderedPoint& point) override;

    void render_texture(const Texture& texture, const TextureRenderParams& options) override;

    void write_to_image(const path& output_path) override;
};

class SfmlRenderer : public BaseRenderer {
private:
    const gradient_options& _options;
    shared_ptr<sf::Shader> _shader;

public:
    explicit SfmlRenderer(const gradient_options& options);

    [[nodiscard]] shared_ptr<BaseRendererContext> create_context(const Observer& observer) const override;

};

#endif //RCSOP_RENDERING_SFML_RENDERER_H
