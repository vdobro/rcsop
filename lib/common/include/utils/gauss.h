#ifndef RCSOP_COMMON_GAUSS_H
#define RCSOP_COMMON_GAUSS_H

#include "observed_point.h"

namespace rcsop::common::utils::gauss {
    struct gauss_options {
        double sigma{};
        double integral_factor{};
        double x_scale{};
        double y_scale{};
    };

    [[nodiscard]] double gauss(double x, double sigma, double mu = 0);

    [[nodiscard]] double get_gauss_integral_factor(const double& sigma);

    [[nodiscard]] observed_factor_func rcs_gaussian_vertical(
            double vertical_spread,
            double sigma);
}

#endif //RCSOP_COMMON_GAUSS_H
