#include "cairo_renderer_context.h"

CairoRendererContext::CairoRendererContext(
        const Observer& observer,
        const gradient_options& options)
        : _options(options) {
    this->_surface = Cairo::ImageSurface::create_from_png(observer.source_image_path());
    this->_cairo_context = Cairo::Context::create(_surface);
}

static const double RGB = 256.0;

void CairoRendererContext::render_point(const rendered_point& point) {
    const auto [coordinates, color] = point;

    const double x = coordinates.x(),
            y = coordinates.y();
    const double r = static_cast<double>(point.color.x()) / RGB,
            g = static_cast<double>(point.color.y()) / RGB,
            b = static_cast<double>(point.color.z()) / RGB;

    const auto& radius = _options.radius;
    auto radial_pattern = Cairo::RadialGradient::create(x, y, 0, x, y, radius);
    radial_pattern->add_color_stop_rgba(0, r, g, b, _options.center_alpha);
    radial_pattern->add_color_stop_rgba(1, r, g, b, 0);

    _cairo_context->save();
    _cairo_context->rectangle(x - radius, y - radius, x + radius, y + radius);
    _cairo_context->clip();
    _cairo_context->set_source(radial_pattern);
    _cairo_context->mask(radial_pattern);
    _cairo_context->restore();
}

void CairoRendererContext::render_texture(const Texture& texture,
                                          const texture_rendering_options& options) {
    auto texture_surface = Cairo::ImageSurface::create_from_png(texture.file_path());
    auto [coordinates, size] = options;

    _cairo_context->save();
    _cairo_context->translate(coordinates.x(), coordinates.y());
    _cairo_context->scale(size.x() / texture_surface->get_width(), size.y() / texture_surface->get_height());
    _cairo_context->set_source(texture_surface, 0, 0);
    _cairo_context->rectangle(0, 0, size.x(), size.y());
    _cairo_context->paint();
}

void CairoRendererContext::write_to_image(const path& output_path) {
    _surface->write_to_png(output_path);
}
