#pragma once

#include <cmath>
#include <util/types.h>

Eigen::Vector3ub map_turbo(double v, double vmin, double vmax);

Eigen::Vector3ub map_jet(double v, double vmin, double vmax);
