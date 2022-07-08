#ifndef RCSOP_RENDERING_BASE_RENDERER_H
#define RCSOP_RENDERING_BASE_RENDERER_H

#include "utils/points.h"

#include "colors.h"
#include "texture.h"
#include "observer.h"

struct RenderedPoint {
    Vector2d coordinates = Vector2d::Zero();
    Vector3ub color = Vector3ub::Zero();
};

struct TextureRenderParams {
    Vector2d coordinates = Vector2d::Zero();
    Vector2d size = Vector2d::Zero();
};

class BaseRendererContext {
public:
    virtual void render_point(const RenderedPoint& point) = 0;
    virtual void render_texture(const Texture& texture, const TextureRenderParams& options) = 0;
    virtual void write_to_image(const path& output_path) = 0;
};

//template<typename TContext = BaseRendererContext> requires std::is_base_of_v<BaseRendererContext, TContext>
class BaseRenderer {
public:
    [[nodiscard]] virtual shared_ptr<BaseRendererContext> create_context(const Observer& observer) const = 0;
};

#endif //RCSOP_RENDERING_BASE_RENDERER_H
