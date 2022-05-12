#pragma once

#include <utility>
#include <algorithm>
#include <filesystem>

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
#define COLOR_MAP map_turbo

#undef SINGLE_IMAGE
#define DEFAULT_IMAGE_ID 1

void render_images(const model_ptr& model,
                   const string& input_path,
                   const string& output_path,
                   const vector<scored_point>& points);
