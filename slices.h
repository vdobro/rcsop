#pragma once

#include <memory>
#include <string>

#include "colmap/base/reconstruction.h"

using std::string;
using colmap::Reconstruction;

void color_slices(std::unique_ptr<Reconstruction> model, const string &output_path);