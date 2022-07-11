#ifndef RCSOP_RENDERING_COLORS_H
#define RCSOP_RENDERING_COLORS_H

#include "utils/types.h"
#include "utils/sparse.h"
#include "utils/mapping.h"

#include "scored_point.h"

namespace rcsop::rendering::coloring {
    using rcsop::common::utils::sparse::Vector3ub;
    using rcsop::common::ScoreRange;

    using local_colormap_func = function<Vector3ub(double v, double vmin, double vmax)>;
    using global_colormap_func = function<Vector3ub(double v)>;

    Vector3ub map_turbo(double v, double vmin, double vmax);

    Vector3ub map_jet(double v, double vmin, double vmax);

    Vector3ub map_red(double v, double vmin, double vmax);

    global_colormap_func construct_colormap_function(
            const local_colormap_func& colormap,
            double min_value,
            double max_value);

    global_colormap_func construct_colormap_function(
            const local_colormap_func& colormap,
            const ScoreRange& range);

    global_colormap_func construct_colormap_function(
            const local_colormap_func& colormap,
            const vector<double>& values);

    vector<Vector3ub> color_values(const vector<double>& value,
                                   const local_colormap_func& colormap);

}

#endif //RCSOP_RENDERING_COLORS_H
