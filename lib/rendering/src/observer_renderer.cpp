#include "observer_renderer.h"

using std::isnan;
using std::invalid_argument;
using std::runtime_error;
using std::make_shared;
using std::cerr;

static const float RGB = 256.f;
static const float RADIUS = 25.f;
static const float CENTER_ALPHA = 0.2f;
static const float GRADIENT_STRENGTH = 3.f;

using namespace sfm::rendering;

static vector<RenderedPoint> project_in_camera_with_color(
        const vector<ImagePoint>& points,
        const camera& camera,
        const global_colormap_func& colormap) {
    return map_vec<ImagePoint, RenderedPoint>(points, [camera, colormap](const ImagePoint& point) {
        auto camera_coordinates = camera.project_from_image(point.coordinates());
        auto score = point.score();
        if (isnan(score)) {
            throw invalid_argument("Score of point is not a number");
        }
        auto color = colormap(point.score());
        return RenderedPoint{
                .coordinates = camera_coordinates,
                .color = color
        };
    });
}

const char* FRAGMENT_SHADER =
        "uniform vec2 screen_res; "
        "uniform vec2 p_center; "
        "uniform vec4 p_color; "
        "uniform float p_radius; "
        "uniform float degree; "
        "float dist(vec2 a, vec2 b) { return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); } "
        "void main() {"
        "vec2 center_coords = vec2(p_center.x, screen_res.y - p_center.y); "
        "vec2 frag_coords = vec2(gl_FragCoord.x - 0.5, gl_FragCoord.y - 0.5); "
        "float d = max(0.f, dist(frag_coords, center_coords) / p_radius); "
        "vec4 center_color = p_color; "
        "vec4 outside_color = vec4(center_color.rgb, 0.0); "
        "float gradient = pow(d, 1.0/degree); "
        "gl_FragColor = mix(center_color, outside_color, gradient); "
        "}";

void ObserverRenderer::initialize_renderer() {
    if (!sf::Shader::isAvailable()) {
        throw runtime_error("Shaders unsupported");
    }
    this->_shader = make_shared<sf::Shader>();
    if (!this->_shader->loadFromMemory(FRAGMENT_SHADER, sf::Shader::Fragment)) {
        throw runtime_error("Could not load required shader.");
    }
}

static void draw_background(
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

vector<RenderedPoint> ObserverRenderer::project_points(
        const global_colormap_func& colormap) {
    const auto& camera = _observer.native_camera();
    auto img_points = camera.project_to_image(*_points);

    std::ranges::sort(img_points, std::ranges::greater(), &ImagePoint::distance);
    auto rendered_points = project_in_camera_with_color(img_points, camera, colormap);
    return rendered_points;
}

static void render_texture(const Texture& texture,
                           const TextureRenderParams& coordinates,
                           sf::RenderTarget& render_target) {
    sf::Texture sf_texture;
    auto texture_path = texture.file_path();
    if (!sf_texture.loadFromFile(texture_path)) {
        throw invalid_argument("Could not load texture " + texture_path.string());
    }
    sf_texture.setSmooth(true);
    sf::RectangleShape shape;
    shape.setTexture(&sf_texture);

    auto x = static_cast<float>(coordinates.coordinates.x());
    auto y = static_cast<float>(coordinates.coordinates.y());
    shape.setPosition(x, y);

    auto width = static_cast<float>(coordinates.size.x());
    auto height = static_cast<float>(coordinates.size.y());
    shape.setSize(sf::Vector2f(width, height));

    render_target.draw(shape);
}

void ObserverRenderer::render_point(
        const RenderedPoint& point,
        sf::RenderTarget& render_target) {
    sf::VertexArray quad(sf::Quads, 4);

    auto x = static_cast<float>(point.coordinates.x());
    auto y = static_cast<float>(point.coordinates.y());

    auto x_begin = x - RADIUS;
    auto x_end = x + RADIUS;
    auto y_begin = y - RADIUS;
    auto y_end = y + RADIUS;

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
    auto screen_size = render_target.getSize();

    auto& shader = *this->_shader;

    shader.setUniform("screen_res", sf::Glsl::Vec2(
            static_cast<float>(screen_size.x),
            static_cast<float>(screen_size.y)));
    shader.setUniform("p_color", sf::Glsl::Vec4(r, g, b, CENTER_ALPHA));
    shader.setUniform("p_center", sf::Glsl::Vec2(x, y));
    shader.setUniform("degree", GRADIENT_STRENGTH);
    shader.setUniform("p_radius", RADIUS);

    render_target.draw(quad, &shader);
}

void ObserverRenderer::render(const path& output_path,
                              const global_colormap_func& colormap,
                              const string& log_prefix) {
    auto time_measure = start_time();

    const auto& camera = _observer.native_camera();
    const path input_file_path = _observer.source_image_path();
    const path output_file_path{output_path / camera.get_name()};
    string output_name = output_file_path.filename().string();

    sf::RenderTexture render_target;
    if (!render_target.create(camera.image_width(), camera.image_height())) {
        throw runtime_error("Could not create render texture.");
    }

    draw_background(render_target, input_file_path);
    vector<RenderedPoint> rendered_points = this->project_points(colormap);

    for (const auto& point: rendered_points) {
        render_point(point, render_target);
    }
    for (const auto& texture_pair: *_textures) {
        render_texture(texture_pair.second, texture_pair.first, render_target);
    }

    render_target.display();

    const auto output_texture = render_target.getTexture();
    auto output_image = output_texture.copyToImage();

    output_image.saveToFile(output_file_path);
    log_and_start_next(time_measure, log_prefix + "\tRendered image " + output_name);
}

ObserverRenderer::ObserverRenderer(const ScoredCloud& points_with_observer)
        : _observer(points_with_observer.observer()),
          _points(points_with_observer.points()) {
    this->initialize_renderer();
    this->_textures = make_shared<vector<pair<TextureRenderParams, Texture>>>();
}

void ObserverRenderer::add_texture(Texture texture, TextureRenderParams coordinates) {
    this->_textures->push_back(make_pair(coordinates, texture));
}
