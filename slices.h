#pragma once

#include <memory>
#include <string>

#include "colmap/base/reconstruction.h"

#include "utils.h"
#include "space_point.h"
#include "colors.h"

using std::string;
using colmap::Reconstruction;

void color_slices(std::unique_ptr<Reconstruction> model,
                  std::vector<double> rcs,
                  const string& output_path);