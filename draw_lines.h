#pragma once
#include <Eigen/Geometry>

#include "colors.h"
#include "utils.h"
#include "types.h"

void draw_lines(const model_ptr& model,
                const vector<double>& rcs,
                const string& input_path,
                const string& output_path);
