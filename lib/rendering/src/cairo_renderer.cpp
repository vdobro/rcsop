#include "cairo_renderer.h"
#include "cairo_renderer_context.h"

using rcsop::rendering::CairoRenderer;
using rcsop::rendering::BaseRendererContext;

CairoRenderer::CairoRenderer(const gradient_options& options)
        : _options(options) {}

shared_ptr<BaseRendererContext> CairoRenderer::create_context(const Observer& observer) const {
    return make_shared<CairoRendererContext>(observer, _options);
}

