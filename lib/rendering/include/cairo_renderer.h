#ifndef RCSOP_RENDERING_CAIRO_RENDERER_H
#define RCSOP_RENDERING_CAIRO_RENDERER_H

#include "base_renderer.h"
#include "rendering_options.h"

namespace rcsop::rendering {
    class CairoRenderer : public BaseRenderer {
    private:
        const gradient_options& _options;

    public:
        explicit CairoRenderer(const gradient_options& options);

        [[nodiscard]] shared_ptr<BaseRendererContext> create_context(const Observer& observer) const override;
    };
}

#endif //RCSOP_RENDERING_CAIRO_RENDERER_H
