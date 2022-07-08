#ifndef RCSOP_RENDERING_OPTIONS_H
#define RCSOP_RENDERING_OPTIONS_H

#include "colors.h"

struct gradient_options {
    float radius;
    float center_alpha;
};

struct rendering_options {
    bool use_gpu_rendering;
    sfm::rendering::local_colormap_func color_map;
    gradient_options gradient;
};

#endif //RCSOP_RENDERING_OPTIONS_H
