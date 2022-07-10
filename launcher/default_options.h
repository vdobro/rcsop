#ifndef RCSOP_LAUNCHER_DEFAULT_OPTIONS_H
#define RCSOP_LAUNCHER_DEFAULT_OPTIONS_H

#include "utils/types.h"
#include "colors.h"

namespace rcsop::launcher {
    const rcsop::rendering::coloring::local_colormap_func COLOR_MAP
            = rcsop::rendering::coloring::map_jet;
    const double DEFAULT_CAMERA_DISTANCE = 750.0;
    const double DEFAULT_CAMERA_PITCH_CORRECTION = -1.;
    const height_t DEFAULT_HEIGHT = 40;
}
#endif //RCSOP_LAUNCHER_DEFAULT_OPTIONS_H
