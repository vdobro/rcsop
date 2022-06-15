#pragma once

#include <utility>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "utils/types.h"
#include "image_point.h"
#include "sparse_cloud.h"
#include "scored_point.h"

#include "colors.h"

#define RGB 256.f
#define RADIUS 25.f
#define CENTER_ALPHA 0.2f
#define GRADIENT_STRENGTH 3.f

namespace sfm::rendering {

struct RenderedPoint {
    Vector2d coordinates = Vector2d::Zero();
    Vector3ub color = Vector3ub::Zero();
};

struct point_display_payload {
    double min_value{};
    double max_value{};
    map<camera_id_t, vector<scored_point>> points;
    path image_path;
};

string get_log_prefix(size_t current, size_t last);

shared_ptr<sf::Shader> initialize_renderer();

void draw_background(sf::RenderTarget& render_target, const path& input_file_path);

global_colormap_func get_colormap(const vector<scored_point>& points,
                                  const local_colormap_func& colormap);

void render_image(
        const camera& camera,
        const shared_ptr<sf::Shader>& point_shader,
        const path& input_path,
        const path& output_path,
        const vector<scored_point>& points,
        const global_colormap_func& colormap,
        const string& log_prefix);

void render_images(const path& input_path,
                   const path& output_path,
                   const vector<camera>& cameras,
                   const vector<scored_point>& points,
                   const global_colormap_func& colormap);

}
