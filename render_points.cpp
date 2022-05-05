#include "render_points.h"

class image_point {
private:
    Vector2d _coordinates = Vector2d::Zero();
    double _score = 0;
public:
    explicit image_point() = default;

    explicit image_point(Vector2d coordinates, double score)
            : _coordinates(std::move(coordinates)), _score(score) {};

    [[nodiscard]] double score() const {
        return _score;
    }

    [[nodiscard]] Vector2d coords() const {
        return _coordinates;
    }
};

static image_point project_to_image(const Image& image,
                                    const scored_point& point) {
    const auto position_normalized = (image.ProjectionMatrix() * point.position().homogeneous()).hnormalized();
    return image_point(position_normalized, point.score());
}

struct rendered_point {
    Vector2d coordinates;
    Vector3ub color;
};

static vector<rendered_point> project_in_camera_with_color(const vector<image_point>& points,
                                                           const Camera& camera,
                                                           colormap_func colormap) {
    auto scores = map_vec<image_point, double>(points, [](const image_point& point) {
        return point.score();
    });
    auto min_score = *std::min_element(scores.begin(), scores.end());
    auto max_score = *std::max_element(scores.begin(), scores.end());

    return map_vec<image_point, rendered_point>(points, [camera, colormap, min_score, max_score]
            (const image_point& point) {
        auto camera_coordinates = camera.WorldToImage(point.coords());
        auto color = colormap(point.score(), min_score, max_score);
        return rendered_point{
                .coordinates = camera_coordinates,
                .color = color
        };
    });
}

#define RGB 256.f
#define RADIUS 25.f
#define CENTER_ALPHA 0.2f
#define GRADIENT_STRENGTH 3.f

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

    quad[0].color = sf::Color::Transparent;
    quad[1].color = sf::Color::Transparent;
    quad[2].color = sf::Color::Transparent;
    quad[3].color = sf::Color::Transparent;

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

void render_images(const model_ptr& model,
                   const string& input_path,
                   const string& output_path,
                   const vector<scored_point>& points) {
    create_directories(output_path);

    if (!sf::Shader::isAvailable()) {
        std::cout << "Shaders unsupported." << std::endl;
        exit(1);
    }
    sf::Shader shader;
    if (!shader.loadFromFile("gradient.frag", sf::Shader::Fragment)) {
        std::cout << "Could not load required shader." << std::endl;
        exit(1);
    }

    auto image_count = model->NumImages();
    for (size_t image_id = 1; image_id <= image_count; image_id++) {
        auto image = model->Image(image_id);
        auto camera_id = image.CameraId();
        auto camera = model->Camera(camera_id);
        auto img_points = map_vec<scored_point, image_point>(points, [image](const scored_point& point) {
            return project_to_image(image, point);
        });
        auto rendered_points = project_in_camera_with_color(img_points, camera, map_turbo);

        auto input_file_path = input_path + path_separator + image.Name();
        auto output_file_path = output_path + path_separator + image.Name();

        sf::RenderTexture render_target;
        if (!render_target.create(camera.Width(), camera.Height())) {
            std::cout << "Could not create render texture." << std::endl;
            exit(2);
        }

        sf::Texture background;
        if (!background.loadFromFile(input_file_path)) {
            std::cout << "Could not load texture " << image.Name() << std::endl;
            exit(3);
        }

        sf::Sprite background_sprite;
        background_sprite.setTexture(background);

        render_target.clear(sf::Color::Black);
        render_target.draw(background_sprite);

        for (const auto& point: rendered_points) {
            render_point(point, render_target, shader);
        }

        render_target.display();

        const auto output_texture = render_target.getTexture();
        auto output_image = output_texture.copyToImage();
        output_image.saveToFile(output_file_path);
    }
}

void render_images(const model_ptr& model,
                   const string& input_path,
                   const string& output_path,
                   const map<point_id_t, scored_point>& points) {
    vector<scored_point> point_list;
    point_list.resize(points.size());

    for (const auto& item: points) {
        point_list.push_back(item.second);
    }

    render_images(model, input_path, output_path, point_list);
}