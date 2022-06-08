#include "render_points.h"

#include "utils/vector.h"
#include "utils/chronometer.h"

using std::string;

namespace sfm::rendering {

typedef struct rendered_point {
    Vector2d coordinates = Vector2d::Zero();
    Vector3ub color = Vector3ub::Zero();
} rendered_point;

static vector<rendered_point> project_in_camera_with_color(
        const vector<image_point>& points,
        const camera& camera,
        const global_colormap_func& colormap) {
    return map_vec<image_point, rendered_point>(points, [camera, colormap](const image_point& point) {
        auto camera_coordinates = camera.project_from_image(point.coords());
        auto score = point.score();
        if (std::isnan(score)) {
            throw std::invalid_argument("score");
        }
        auto color = colormap(point.score());
        return rendered_point{
                .coordinates = camera_coordinates,
                .color = color
        };
    });
}

static void render_point(
        const rendered_point& point,
        sf::RenderTarget& render_target,
        sf::Shader& shader) {
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

    shader.setUniform("screen_res", sf::Glsl::Vec2(
            static_cast<float>(screen_size.x),
            static_cast<float>(screen_size.y)));
    shader.setUniform("p_color", sf::Glsl::Vec4(r, g, b, CENTER_ALPHA));
    shader.setUniform("p_center", sf::Glsl::Vec2(x, y));
    shader.setUniform("degree", GRADIENT_STRENGTH);
    shader.setUniform("p_radius", RADIUS);

    render_target.draw(quad, &shader);
}

string get_log_prefix(size_t current, size_t last) {
    int width = ceil(log10(static_cast<double>(last)));
    std::stringstream ss;
    ss << "(" << std::setw(width) << std::to_string(current)
       << "/" << std::setw(width) << std::to_string(last)
       << ")\t ";
    return ss.str();
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

shared_ptr<sf::Shader> initialize_renderer() {
    if (!sf::Shader::isAvailable()) {
        std::cout << "Shaders unsupported." << std::endl;
        exit(1);
    }
    shared_ptr<sf::Shader> shader = std::make_shared<sf::Shader>();
    if (!shader->loadFromMemory(FRAGMENT_SHADER, sf::Shader::Fragment)) {
        std::cout << "Could not load required shader." << std::endl;
        exit(1);
    }
    return shader;
}

static void draw_background(
        sf::RenderTarget& render_target,
        const path& input_file_path) {
    sf::Texture background;
    if (!background.loadFromFile(input_file_path)) {
        std::cerr << "Could not load texture " << input_file_path << std::endl;
        exit(3);
    }

    sf::Sprite background_sprite;
    background_sprite.setTexture(background);

    render_target.clear(sf::Color::Black);
    render_target.draw(background_sprite);
}

static vector<rendered_point> project_points(const camera& camera,
                                             const vector<scored_point>& points,
                                             const global_colormap_func& colormap) {
    auto img_points = camera.project_to_image(points);

    std::ranges::sort(img_points, std::ranges::greater(), &image_point::distance);
    auto rendered_points = project_in_camera_with_color(img_points, camera, colormap);
    return rendered_points;
}

void render_image(
        const SparseCloud& model,
        const camera& camera,
        const shared_ptr<sf::Shader>& point_shader,
        const path& input_path,
        const path& output_path,
        const vector<scored_point>& points,
        const global_colormap_func& colormap,
        const string& log_prefix) {
    const path input_file_path{input_path / camera.get_name()};
    const path output_file_path{output_path / camera.get_name()};

    auto time_measure = start_time();

    sf::RenderTexture render_target;
    if (!render_target.create(camera.width(), camera.height())) {
        std::cerr << "Could not create render texture." << std::endl;
        exit(2);
    }

    draw_background(render_target, input_file_path);
    time_measure = log_and_start_next(time_measure,
                                      log_prefix + "\tSetting up rendering pipeline for image " +
                                      input_file_path.string());
    vector<rendered_point> rendered_points = project_points(camera, points, colormap);

    time_measure = log_and_start_next(time_measure,
                                      log_prefix + "\tPreparing coloring and projection");
    for (const auto& point: rendered_points) {
        render_point(point, render_target, *point_shader);
    }

    render_target.display();
    time_measure = log_and_start_next(time_measure, log_prefix + "\tRendering image");

    const auto output_texture = render_target.getTexture();
    auto output_image = output_texture.copyToImage();
    time_measure = log_and_start_next(time_measure, log_prefix + "\tRetrieving image from the GPU");

    output_image.saveToFile(output_file_path);
    log_and_start_next(time_measure, log_prefix + "\tSaving image " + camera.get_name());
}

void render_images(const SparseCloud& model,
                   const path& input_path,
                   const path& output_path,
                   const vector<scored_point>& points,
                   const global_colormap_func& colormap) {
    auto shader = initialize_renderer();

    create_directories(output_path);
#ifdef SINGLE_IMAGE
    camera_id_t first_camera = DEFAULT_IMAGE_ID;
    camera_id_t last_camera = first_camera;
#else
    camera_id_t first_camera = 1;
    camera_id_t last_camera = model.get_cameras().size();
#endif
    for (camera_id_t camera_id = first_camera; camera_id <= last_camera; camera_id++) {
        auto camera = model.find_camera(camera_id);
        auto log_prefix = get_log_prefix(camera_id, last_camera);

        render_image(model, camera,
                     shader, input_path, output_path,
                     points, colormap, log_prefix);

    }
}

global_colormap_func get_colormap(const vector<scored_point>& points,
                                  const local_colormap_func& colormap) {
    auto scores = map_vec<scored_point, double>(points, [](const scored_point& point) {
        return point.score_to_dB();
    });

    auto min_value = *std::min_element(scores.begin(), scores.end());
    auto max_value = *std::max_element(scores.begin(), scores.end());
    std::cerr << "Min " << min_value << " Max " << max_value << std::endl;
    return construct_colormap_function(colormap, min_value, max_value);
}

}
