#pragma once

#include <filesystem>

#include "types.h"
#include "scored_point.h"

typedef map<point_id_t, scored_point> scored_point_map;

shared_ptr<Reconstruction> read_model(const string& path);

void write_model(const model_ptr& model, const string& output_path);

vector<Image> get_images(const Reconstruction& model);

vector<point_pair> get_points(const Reconstruction& model);

double get_world_scale(double camera_distance_to_origin, const Reconstruction& model);

scored_point_map get_scored_points(const Reconstruction& model);