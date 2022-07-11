#include "sfml_renderer_context.h"

namespace rcsop::rendering {
    using rcsop::rendering::SfmlRendererContext;

    static const float RGB = 256.f;

    static inline void fill_background(
            sf::RenderTarget& render_target,
            const path& input_file_path) {
        sf::Texture background;
        if (!background.loadFromFile(input_file_path)) {
            throw runtime_error("Could not load texture " + input_file_path.string());
        }

        sf::Sprite background_sprite;
        background_sprite.setTexture(background);

        render_target.clear(sf::Color::Black);
        render_target.draw(background_sprite);
    }

    SfmlRendererContext::SfmlRendererContext(const Observer& observer,
                                             shared_ptr<sf::Shader> shader,
                                             const gradient_options& options)
            : _options(options),
              _shader(shader) {
        const auto& camera = observer.native_camera();
        this->_render_target = make_unique<sf::RenderTexture>();
        if (!this->_render_target->create(camera.image_width(), camera.image_height())) {
            throw runtime_error("Could not create render texture.");
        }
        fill_background(*_render_target, observer.source_image_path());
    }

    void SfmlRendererContext::render_point(const rendered_point& point) {
        sf::VertexArray quad(sf::Quads, 4);

        auto x = static_cast<float>(point.coordinates.x());
        auto y = static_cast<float>(point.coordinates.y());

        auto x_begin = x - _options.radius;
        auto x_end = x + _options.radius;
        auto y_begin = y - _options.radius;
        auto y_end = y + _options.radius;

        quad[0].position = sf::Vector2f(x_begin, y_begin);
        quad[1].position = sf::Vector2f(x_end, y_begin);
        quad[2].position = sf::Vector2f(x_end, y_end);
        quad[3].position = sf::Vector2f(x_begin, y_end);

        const auto quad_corner_color = sf::Color::Transparent;
        quad[0].color = quad_corner_color;
        quad[1].color = quad_corner_color;
        quad[2].color = quad_corner_color;
        quad[3].color = quad_corner_color;

        const auto r = static_cast<float>(point.color.x()) / RGB,
                g = static_cast<float>(point.color.y()) / RGB,
                b = static_cast<float>(point.color.z()) / RGB;
        auto screen_size = this->_render_target->getSize();

        sf::Shader& shader = *this->_shader;
        shader.setUniform("screen_res", sf::Glsl::Vec2(
                static_cast<float>(screen_size.x),
                static_cast<float>(screen_size.y)));
        shader.setUniform("p_color", sf::Glsl::Vec4(r, g, b, _options.center_alpha));
        shader.setUniform("p_center", sf::Glsl::Vec2(x, y));
        shader.setUniform("degree", 3.0f);
        shader.setUniform("p_radius", _options.radius);

        _render_target->draw(quad, &shader);
    }

    void SfmlRendererContext::render_texture(
            const Texture& texture,
            const texture_rendering_options& options) {

        sf::Texture sf_texture;
        auto texture_path = texture.file_path();
        if (!sf_texture.loadFromFile(texture_path)) {
            throw invalid_argument("Could not load texture " + texture_path.string());
        }
        sf_texture.setSmooth(true);
        sf::RectangleShape shape;
        shape.setTexture(&sf_texture);

        auto x = static_cast<float>(options.coordinates.x());
        auto y = static_cast<float>(options.coordinates.y());
        shape.setPosition(x, y);

        auto width = static_cast<float>(options.size.x());
        auto height = static_cast<float>(options.size.y());
        shape.setSize(sf::Vector2f(width, height));

        _render_target->draw(shape);
    }

    void SfmlRendererContext::write_to_image(const path& output_path) {
        _render_target->display();

        const auto output_texture = _render_target->getTexture();
        auto output_image = output_texture.copyToImage();

        output_image.saveToFile(output_path);
    }
}
