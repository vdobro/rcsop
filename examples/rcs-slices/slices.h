#pragma once

#include "utils/types.h"
#include "sparse_cloud.h"

void color_slices(sparse_cloud& model,
                  const vector<double>& rcs,
                  const path& input_path,
                  const path& output_path);