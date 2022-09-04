#ifndef RCSOP_RENDERING_COLORS_H
#define RCSOP_RENDERING_COLORS_H

#include "utils/types.h"
#include "utils/sparse.h"
#include "utils/mapping.h"

#include "scored_point.h"

namespace rcsop::common::coloring {
    using rcsop::common::utils::sparse::color_vec;
    using rcsop::common::ScoreRange;

    using local_colormap_func = function<color_vec(double v, double vmin, double vmax)>;
    using global_colormap_func = function<color_vec(double v)>;

    [[nodiscard]] color_vec map_turbo(double v, double vmin, double vmax);

    [[nodiscard]] color_vec map_jet(double v, double vmin, double vmax);

    [[nodiscard]] color_vec map_red(double v, double vmin, double vmax);

    [[nodiscard]] color_vec map_red_alpha_only(double v, double vmin, double vmax);

    [[nodiscard]] local_colormap_func resolve_map_by_name(const string& name);

    [[nodiscard]] global_colormap_func construct_color_map_function(
            const local_colormap_func& color_map,
            double min_value,
            double max_value);

    [[nodiscard]] global_colormap_func construct_color_map_function(
            const local_colormap_func& color_map,
            const ScoreRange& range);

    [[nodiscard]] vector<color_vec> color_values(const vector<double>& value,
                                                 const local_colormap_func& colormap);

}

#endif //RCSOP_RENDERING_COLORS_H
