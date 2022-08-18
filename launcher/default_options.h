#ifndef RCSOP_LAUNCHER_DEFAULT_OPTIONS_H
#define RCSOP_LAUNCHER_DEFAULT_OPTIONS_H

#include "utils/types.h"
#include "colors.h"

namespace rcsop::launcher {
    using rcsop::common::height_t;

    using rcsop::common::coloring::local_colormap_func;
    using rcsop::common::coloring::map_jet;

    const string DEFAULT_COLOR_MAP = "jet";
    const double DEFAULT_CAMERA_DISTANCE = 750.0;
    const double DEFAULT_CAMERA_PITCH_CORRECTION = 0.;
    const height_t DEFAULT_HEIGHT = 40;
}
#endif //RCSOP_LAUNCHER_DEFAULT_OPTIONS_H
