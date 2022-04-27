#pragma once

#include <base/reconstruction.h>

#include "types.h"

void filter_points(const shared_ptr<colmap::Reconstruction>& model,
                   const string& output_path);
