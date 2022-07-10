#ifndef RCSOP_RENDERING_SFML_RENDERER_H
#define RCSOP_RENDERING_SFML_RENDERER_H

#include <SFML/Graphics.hpp>

#include "utils/types.h"

#include "base_renderer.h"
#include "observer.h"
#include "texture.h"
#include "rendering_options.h"

namespace rcsop::rendering {
    class SfmlRenderer : public BaseRenderer {
    private:
        const gradient_options& _options;
        shared_ptr<sf::Shader> _shader;

    public:
        explicit SfmlRenderer(const gradient_options& options);

        [[nodiscard]] shared_ptr<BaseRendererContext> create_context(const Observer& observer) const override;

    };
}

#endif //RCSOP_RENDERING_SFML_RENDERER_H
