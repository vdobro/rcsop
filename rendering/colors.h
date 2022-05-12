#pragma once

#include <cmath>

#include "utils/types.h"
#include "utils/utils.h"

typedef const std::function<Vector3ub(double v, double vmin, double vmax)>& colormap_func;

Vector3ub map_turbo(double v, double vmin, double vmax);

Vector3ub map_jet(double v, double vmin, double vmax);

Vector3ub map_red(double v, double vmin, double vmax);

vector<Vector3ub> color_values(const vector<double>& values,
                               const std::function<Vector3ub(double v, double vmin, double vmax)>& colormap);