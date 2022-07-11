#ifndef RCSOP_RENDERING_BASE_RENDERER_H
#define RCSOP_RENDERING_BASE_RENDERER_H

#include "utils/points.h"

#include "colors.h"
#include "texture.h"
#include "observer.h"

namespace rcsop::rendering {
    using rcsop::common::utils::points::Vector2d;
    using rcsop::common::utils::sparse::Vector3ub;
    using rcsop::common::Observer;
    using rcsop::common::Texture;

    struct rendered_point {
        Vector2d coordinates = Vector2d::Zero();
        Vector3ub color = Vector3ub::Zero();
    };

    struct texture_rendering_options {
        Vector2d coordinates = Vector2d::Zero();
        Vector2d size = Vector2d::Zero();
    };

    class BaseRendererContext {
    public:
        virtual void render_point(const rendered_point& point) = 0;

        virtual void render_texture(const Texture& texture,
                                    const texture_rendering_options& options) = 0;

        virtual void write_to_image(const path& output_path) = 0;
    };

    class BaseRenderer {
    public:
        [[nodiscard]] virtual shared_ptr<BaseRendererContext> create_context(const Observer& observer) const = 0;
    };
}

#endif //RCSOP_RENDERING_BASE_RENDERER_H
