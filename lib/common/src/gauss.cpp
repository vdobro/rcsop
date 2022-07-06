#include "utils/gauss.h"

#include <cmath>

static double raw_gauss(const double& x,
                        const double& y,
                        const double& sigma) {
    return (M_SQRT1_2 / (sigma * sqrt(M_PI))) * exp(-0.5 * (x * x + y * y) / (sigma * sigma));
}

static double raw_gauss(const double x,
                        const double& sigma) {
    auto exponent_to_square = (x / sigma);
    return (M_SQRT1_2 / (sigma * sqrt(M_PI))) * exp(-0.5 * exponent_to_square * exponent_to_square);
}

#define HORIZONTAL_SPREAD (45.0 / 2.0)
#define VERTICAL_SPREAD (5.0 / 2.0)

static double calc_gauss(const observed_point& point,
                         const gauss_options& distribution_options) {
    return raw_gauss(point.horizontal_angle / distribution_options.x_scale,
                     point.vertical_angle / distribution_options.y_scale,
                     distribution_options.sigma)
           * distribution_options.integral_factor
           / (distribution_options.x_scale * distribution_options.y_scale);
}

static bool is_inside_vertical_range(const observed_point& point, const gauss_options& options) {
    auto y = point.vertical_angle / options.y_scale;
    return y <= 1;
}

static bool is_inside_ellipse(const observed_point& point, const gauss_options& options) {
    auto x = point.horizontal_angle / options.x_scale;
    auto y = point.vertical_angle / options.y_scale;
    return x * x + y * y <= 1;
}

double get_gauss_integral_factor(const double& sigma) {
    auto erf_part = erf(1 / (sigma * M_SQRT2));
    auto integral = sigma * M_SQRT2 * sqrt(M_PI) * erf_part * erf_part;
    return 1 / integral;
}

double rcs_gaussian(const observed_point& point,
                    const gauss_options& options) {
    if (!is_inside_ellipse(point, options)) {
        return 0;
    }

    return calc_gauss(point, options) / point.distance_in_world;
}

static const double STIGLER_SIGMA_SQ = M_2_SQRTPI * M_SQRT1_2 / 2;
double rcs_gaussian_vertical(const observed_point& point) {
    return raw_gauss(point.vertical_angle / VERTICAL_SPREAD, STIGLER_SIGMA_SQ);
}