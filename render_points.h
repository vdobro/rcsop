#pragma once

#include "types.h"
#include "scored_point.h"

void render_to_image(const model_ptr& model,
                     colmap::image_t image_id,
                     const string& input_path,
                     const string& output_path,
                     const vector<scored_point>& points);