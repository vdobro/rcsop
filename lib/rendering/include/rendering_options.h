#ifndef RCSOP_RENDERING_OPTIONS_H
#define RCSOP_RENDERING_OPTIONS_H

#include "colors.h"

namespace rcsop::rendering {
    struct gradient_options {
        float radius;
        float center_alpha;
    };

    struct rendering_options {
        bool use_gpu_rendering;
        rcsop::rendering::coloring::local_colormap_func color_map;
        rcsop::rendering::gradient_options gradient;
    };
}

#endif //RCSOP_RENDERING_OPTIONS_H
