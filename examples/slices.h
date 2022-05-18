#pragma once

#include "utils/projection.h"

#include "scored_point.h"

#include "rendering/colors.h"
#include "rendering/render_points.h"

#include "options.h"

void color_slices(const shared_ptr<Reconstruction>& model,
                  const vector<double>& rcs,
                  const string& input_path,
                  const string& output_path);