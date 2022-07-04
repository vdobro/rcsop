#include "utils/gauss.h"

#include <cmath>

static double raw_gauss(const double& x,
                        const double& y,
                        const double& sigma) {
    return M_SQRT1_2 * exp(-1 * (x * x + y * y) / (2 * sigma * sigma)) / (sigma * sqrt(M_PI));
}


#define HORIZONTAL_SPREAD (45.0 / 2.0)
#define VERTICAL_SPREAD (15.0 / 2.0)
#define GAUSS_SIGMA_2 (3 * 3)

//(numeric) sum of Riemann integral of 1 / (exp (-(3x)^2)) from -3 to +3
#define GAUSS_INTEGRAL_FACTOR 1.69260614115415

static double calc_gauss_vertical(const observed_point& point,
                                  const gauss_options& distribution_options,
                                  const double& sigma) {
    const auto value = point.vertical_angle;
    const auto exp_arg = -1 * GAUSS_SIGMA_2 * value * value / distribution_options.y_scale;
    const auto result = exp(exp_arg) * GAUSS_INTEGRAL_FACTOR / HORIZONTAL_SPREAD;
    return result;
}

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

double rcs_gaussian_vertical(const observed_point& point,
                             const gauss_options& options) {
    if (!is_inside_ellipse(point, options)) {
        return 0;
    }

    return calc_gauss(point, options) / point.distance_in_world;
}