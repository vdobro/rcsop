#pragma once

#include <Eigen/Geometry>

#include "utils/types.h"
#include "utils/model.h"
#include "utils/projection.h"

#include "rendering/colors.h"

void draw_lines(const model_ptr& model,
                const vector<double>& rcs,
                const path& input_path,
                const path& output_path);
