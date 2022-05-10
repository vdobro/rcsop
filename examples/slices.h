#pragma once

#include "scored_point.h"
#include "utils/utils.h"
#include "rendering/colors.h"
#include "rendering/render_points.h"

void color_slices(const shared_ptr<colmap::Reconstruction>& model,
                  const vector<double>& rcs,
                  const string& input_path,
                  const string& output_path);