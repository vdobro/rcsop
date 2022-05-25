#pragma once

#include "types.h"
#include "scored_point.h"

shared_ptr<Reconstruction> read_model(const path& path);

void write_model(const model_ptr& model, const path& output_path);

vector<Image> get_images(const Reconstruction& model);

vector<point_pair> get_points(const Reconstruction& model);

scored_point_map get_scored_points(const Reconstruction& model);