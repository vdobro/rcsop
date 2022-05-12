#pragma once

#include <Eigen/Geometry>

#include "rendering/colors.h"
#include "utils/model.h"
#include "utils/projection.h"
#include "utils/types.h"

void draw_lines(const model_ptr& model,
                const vector<double>& rcs,
                const string& input_path,
                const string& output_path);
