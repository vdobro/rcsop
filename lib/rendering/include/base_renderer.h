#ifndef RCSOP_RENDERING_BASE_RENDERER_H
#define RCSOP_RENDERING_BASE_RENDERER_H

#include "utils/points.h"

#include "colors.h"
#include "texture.h"
#include "observer.h"

namespace rcsop::rendering {
    using rcsop::common::utils::points::vec2;
    using rcsop::common::utils::sparse::color_vec;
    using rcsop::common::Observer;
    using rcsop::common::Texture;

    struct rendered_point {
        vec2 coordinates = vec2::Zero();
        float size_factor = 1;
        color_vec color = color_vec::Zero();
    };

    struct texture_rendering_options {
        vec2 coordinates = vec2::Zero();
        vec2 size = vec2::Zero();
    };

    class BaseRendererContext {
    public:
        virtual void render_point(const rendered_point& point) = 0;

        virtual void render_texture(const Texture& texture,
                                    const texture_rendering_options& options) = 0;

        virtual void write_to_image(const path& output_path) = 0;

        virtual ~BaseRendererContext() = default;
    };

    class BaseRenderer {
    public:
        [[nodiscard]] virtual shared_ptr<BaseRendererContext> create_context(const Observer& observer) const = 0;

        virtual ~BaseRenderer() = default;
    };
}

#endif //RCSOP_RENDERING_BASE_RENDERER_H
