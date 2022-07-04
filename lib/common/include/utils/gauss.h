#ifndef RCSOP_COMMON_GAUSS_H
#define RCSOP_COMMON_GAUSS_H

#include "observed_point.h"

struct gauss_options {
    double sigma;
    double integral_factor;
    double x_scale;
    double y_scale;
};

double get_gauss_integral_factor(const double& sigma);

double rcs_gaussian(const observed_point& point,
                    const gauss_options& options);

#endif //RCSOP_COMMON_GAUSS_H