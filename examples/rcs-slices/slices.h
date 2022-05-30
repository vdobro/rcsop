#pragma once

#include "../../common/utils/types.h"
#include "../../common/sparse_cloud.h"

void color_slices(sparse_cloud& model,
                  const vector<double>& rcs,
                  const path& input_path,
                  const path& output_path);