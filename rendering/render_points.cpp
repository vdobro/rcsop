#include "render_points.h"

using std::string;

static image_point project_to_image(const Image& image,
                                    const Vector3d& image_position,
                                    const scored_point& point) {
    auto position_normalized = (image.ProjectionMatrix() * point.position().homogeneous()).hnormalized();
    double distance_to_camera = (image_position - point.position()).norm();
    return image_point(position_normalized, distance_to_camera, point.score_to_dB());
}

struct rendered_point {
    Vector2d coordinates = Vector2d::Zero();
    Vector3ub color = Vector3ub::Zero();
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

static string get_log_prefix(size_t current, size_t last) {
    int width = ceil(log10(static_cast<double>(last)));
    std::stringstream ss;
    ss << "(" << std::setw(width) << std::to_string(current)
       << "/" << std::setw(width) << std::to_string(last)
       << ")\t ";
    return ss.str();
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
    if (!shader.loadFromFile("rendering/gradient.frag", sf::Shader::Fragment)) {
        std::cout << "Could not load required shader." << std::endl;
        exit(1);
    }

#ifdef SINGLE_IMAGE
    size_t first_image = DEFAULT_IMAGE_ID;
    size_t last_image = first_image;
#else
    size_t first_image = 1;
    size_t last_image = model->NumImages();
#endif
    for (size_t image_id = first_image; image_id <= last_image; image_id++) {
        auto image = model->Image(image_id);
        auto log_prefix = get_log_prefix(image_id, last_image);

        auto camera_id = image.CameraId();
        auto camera = model->Camera(camera_id);

        auto input_file_path = input_path + path_separator + image.Name();
        auto output_file_path = output_path + path_separator + image.Name();

        auto time_measure = start_time();
        sf::RenderTexture render_target;
        if (!render_target.create(camera.Width(), camera.Height())) {
            std::cerr << "Could not create render texture." << std::endl;
            exit(2);
        }

        sf::Texture background;
        if (!background.loadFromFile(input_file_path)) {
            std::cerr << "Could not load texture " << image.Name() << std::endl;
            exit(3);
        }

        sf::Sprite background_sprite;
        background_sprite.setTexture(background);

        render_target.clear(sf::Color::Black);
        render_target.draw(background_sprite);
        time_measure = log_and_start_next(time_measure,
                                          log_prefix + "Setting up rendering pipeline for image " + image.Name());

        auto image_position = get_image_position(image);
        auto img_points = map_vec<scored_point, image_point>(
                points,
                [image, image_position](const scored_point& point) {
                    return project_to_image(image, image_position, point);
                });

        std::ranges::sort(img_points, std::ranges::greater(), &image_point::distance);
        auto rendered_points = project_in_camera_with_color(img_points, camera, COLOR_MAP);

        time_measure = log_and_start_next(time_measure, log_prefix + "\tPreparing coloring and projection");
        for (const auto& point: rendered_points) {
            render_point(point, render_target, shader);
        }

        render_target.display();
        time_measure = log_and_start_next(time_measure, log_prefix + "\tRendering image");

        const auto output_texture = render_target.getTexture();
        auto output_image = output_texture.copyToImage();
        time_measure = log_and_start_next(time_measure, log_prefix + "\tRetrieving image from the GPU");

        output_image.saveToFile(output_file_path);
        log_and_start_next(time_measure, log_prefix + "\tSaving image " + image.Name());
    }
}
