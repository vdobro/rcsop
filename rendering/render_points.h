#pragma once

#include <utility>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "utils/types.h"
#include "utils/utils.h"
#include "utils/projection.h"

#include "scored_point.h"
#include "image_point.h"
#include "colors.h"

#define RGB 256.f
#define RADIUS 25.f
#define CENTER_ALPHA 0.2f
#define GRADIENT_STRENGTH 3.f

#undef SINGLE_IMAGE
#define DEFAULT_IMAGE_ID 1

using colmap::image_t;
struct point_display_payload {
    double min_value{};
    double max_value{};
    map<image_t, vector<scored_point>> points;
    model_ptr model;
    path image_path;
};

string get_log_prefix(size_t current, size_t last);

shared_ptr<sf::Shader> initialize_renderer();

global_colormap_func get_colormap(const vector<scored_point>& points,
                                  const local_colormap_func& colormap);

void render_image(
        const model_ptr& model,
        const Image& image,
        const shared_ptr<sf::Shader>& point_shader,
        const path& input_path,
        const path& output_path,
        const vector<scored_point>& points,
        const global_colormap_func& colormap,
        const string& log_prefix);

void render_images(const model_ptr& model,
                   const path& input_path,
                   const path& output_path,
                   const vector<scored_point>& points,
                   const global_colormap_func& colormap);