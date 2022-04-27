#include "render_points.h"

#include <utility>
#include <filesystem>
#include <map>

#include "colmap/util/types.h"

#include "cairomm/context.h"
#include "cairomm/surface.h"

#include "colors.h"

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

#define RADIUS 20
#define DOWNSCALER 1
#define ALPHA 0.5

static void draw_point(const Cairo::RefPtr<Cairo::Context>& cr,
                       const rendered_point& point) {
    const auto [coords, color] = point;
    const double x = coords.x(),
            y = coords.y();
    const double r = color.x() / 256.0,
            g = color.y() / 256.0,
            b = color.z() / 256.0;

    auto radial_pattern = Cairo::RadialGradient::create(x, y, 0, x, y, RADIUS);
    radial_pattern->add_color_stop_rgba(0, r, g, b, ALPHA);
    radial_pattern->add_color_stop_rgba(1, r, g, b, 0);

    cr->save();
    cr->rectangle(x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);
    cr->clip();
    cr->set_source(radial_pattern);
    cr->mask(radial_pattern);
    cr->restore();
}

typedef uint64_t rgb_hash;
typedef vector<Vector2d> point2d_list;
using std::map;

#define RGB_SIZE 256
/*
static void draw_points(const Cairo::RefPtr<Cairo::Context>& cr,
                        const map<rgb_hash, point2d_list>& color_map) {

    for (auto it = color_map.begin(); it != color_map.end(); it++) {
        auto hash = it->first;
        auto points2d = it->second;

        Vector3ub color;
        color.x() = hash / (RGB_SIZE * RGB_SIZE);
        color.y() = hash / RGB_SIZE % RGB_SIZE;
        color.z() = hash % RGB_SIZE;
        const double r = color.x() / 256.0,
                g = color.y() / 256.0,
                b = color.z() / 256.0;

        for (const auto & point : points2d) {
            const double x = point.x(),y = point.y();
            auto radial_pattern = Cairo::RadialGradient::create(x, y, 0, x, y, RADIUS);
            radial_pattern->add_color_stop_rgba(0, r, g, b, ALPHA);
            radial_pattern->add_color_stop_rgba(1, r, g, b, 0);
        }
    }
}*/

static rgb_hash rgb_to_hex(const Vector3ub& color) {
    return color.x() * 256 * 256 + color.y() * 256 + color.z();
}

void render_to_image(const model_ptr& model,
                     colmap::image_t image_id,
                     const string& input_path,
                     const string& output_path,
                     const vector<scored_point>& points) {
    auto image = model->Image(image_id);
    auto camera_id = image.CameraId();
    auto camera = model->Camera(camera_id);

    auto img_points = map_vec<scored_point, image_point>(points, [image](const scored_point& point) {
        return project_to_image(image, point);
    });

    auto rendered_points = project_in_camera_with_color(img_points, camera, map_turbo);

    auto surface = Cairo::ImageSurface::create_from_png(input_path + path_separator + image.Name());
    auto cr = Cairo::Context::create(surface);

    map<rgb_hash, point2d_list> color_map;
    for (const auto& rendered_point: rendered_points) {
        auto [coordinates, color] = rendered_point;
        auto hash = rgb_to_hex(color);
        point2d_list grouped_points;
        if (color_map.find(hash) == color_map.end()) {
            color_map.insert(std::pair<rgb_hash, point2d_list>(hash, grouped_points));
        } else {
            grouped_points = color_map[hash];
        }
        grouped_points.push_back(coordinates);
    }

    for (size_t i = 0; i < rendered_points.size(); i += DOWNSCALER) {
        const auto point = rendered_points[i];
        draw_point(cr, point);
    }

    create_directories(output_path);
    string filename = output_path + path_separator + image.Name();
    surface->write_to_png(filename);
}
