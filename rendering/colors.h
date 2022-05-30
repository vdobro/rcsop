#pragma once

#include "../common/utils/types.h"
#include "../common/utils/colmap.h"
#include "../common/utils/vector.h"

namespace sfm::rendering {
    typedef std::function<Vector3ub(double v, double vmin, double vmax)> local_colormap_func;
    typedef std::function<Vector3ub(double v)> global_colormap_func;

    Vector3ub map_turbo(double v, double vmin, double vmax);

    Vector3ub map_jet(double v, double vmin, double vmax);

    Vector3ub map_red(double v, double vmin, double vmax);

    global_colormap_func construct_colormap_function(
            const local_colormap_func& colormap,
            double min_value,
            double max_value);

    global_colormap_func construct_colormap_function(
            const local_colormap_func& colormap,
            const vector<double>& values);

    vector<Vector3ub> color_values(const vector<double>& value,
                                   const local_colormap_func& colormap);

}