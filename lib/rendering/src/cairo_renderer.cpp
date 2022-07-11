#include "cairo_renderer.h"
#include "cairo_renderer_context.h"

namespace rcsop::rendering {
    CairoRenderer::CairoRenderer(const gradient_options& options)
            : _options(options) {}

    shared_ptr<BaseRendererContext> CairoRenderer::create_context(const Observer& observer) const {
        return make_shared<CairoRendererContext>(observer, _options);
    }
}
