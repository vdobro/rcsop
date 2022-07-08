#ifndef RCSOP_RENDERING_CAIRO_RENDERER_CONTEXT_H
#define RCSOP_RENDERING_CAIRO_RENDERER_CONTEXT_H

#include "cairomm/context.h"
#include "cairomm/surface.h"

#include "base_renderer.h"
#include "rendering_options.h"

class CairoRendererContext : public BaseRendererContext {
private:
    const gradient_options _options;

    Cairo::RefPtr<Cairo::Surface> _surface;
    Cairo::RefPtr<Cairo::Context> _cairo_context;
public:
    explicit CairoRendererContext(const Observer& observer,
                                  const gradient_options& options);

    void render_point(const rendered_point& point) override;
    void render_texture(const Texture& texture, const texture_rendering_options& options) override;
    void write_to_image(const path& output_path) override;

};

#endif //RCSOP_RENDERING_CAIRO_RENDERER_CONTEXT_H
