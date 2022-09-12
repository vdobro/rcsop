#ifndef RCSOP_LAUNCHER_DEFAULT_OPTIONS_H
#define RCSOP_LAUNCHER_DEFAULT_OPTIONS_H

#include "utils/types.h"
#include "colors.h"

namespace rcsop::launcher {
    using rcsop::common::height_t;

    using rcsop::common::coloring::local_colormap_func;
    using rcsop::common::coloring::map_jet;

    constexpr double STIGLER_SIGMA = M_2_SQRTPI * M_SQRT1_2 / 2;

    const string DEFAULT_TASK = "azimuth-rcs";
    const string DEFAULT_POINT_GENERATOR = "data-projection";
    const string DEFAULT_OUTPUT_FORMAT = "all";
    const size_t DEFAULT_POINT_DENSITY = 3;

    constexpr double DEFAULT_CAMERA_DISTANCE = 750.0;
    constexpr height_t DEFAULT_HEIGHT = 40;

    constexpr double DEFAULT_VERTICAL_ANGLE_SPREAD = 5.0;
    constexpr double DEFAULT_VERTICAL_DISTRIBUTION_VARIANCE = STIGLER_SIGMA * STIGLER_SIGMA;
    constexpr double DEFAULT_CAMERA_PITCH_CORRECTION = 0.;

    constexpr double DEFAULT_MIN_DB = -20.;
    constexpr double DEFAULT_MAX_DB = 5.;

    const string DEFAULT_COLOR_MAP = "jet";
    constexpr float DEFAULT_ALPHA = 0.3;
    constexpr float DEFAULT_GRADIENT_RADIUS = 15.;
}

#endif //RCSOP_LAUNCHER_DEFAULT_OPTIONS_H
