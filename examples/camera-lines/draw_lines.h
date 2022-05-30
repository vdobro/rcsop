#pragma once

#include <Eigen/Geometry>

#include "utils/types.h"
#include "sparse_cloud.h"

#include <vector>
using std::vector;

void draw_lines(sparse_cloud& model,
                const vector<double>& rcs,
                const path& input_path,
                const path& output_path);
