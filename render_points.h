#pragma once

#include "types.h"
#include "scored_point.h"

void render_images(const model_ptr& model,
                   const string& input_path,
                   const string& output_path,
                   const vector<scored_point>& points);

void render_images(const model_ptr& model,
                   const string& input_path,
                   const string& output_path,
                   const map<point_id_t, scored_point>& points);