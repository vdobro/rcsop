#pragma once

#include <string>

#include "colmap/base/reconstruction.h"

#include "utils.h"

using std::unique_ptr;
using std::string;

void filter_points(unique_ptr<colmap::Reconstruction> model, const string& output_path);
