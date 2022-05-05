#include <string>

#include "utils.h"
#include "cleanup.h"
#include "mat_reader.h"
#include "draw_lines.h"
#include "rcs_sums.h"
#include "slices.h"

#undef FILTER_POINTS
#undef DRAW_LINES
#undef SLICE_POINTS
#define SUM_PYRAMIDS

int main() {
    const string data_root_path = "data";
    const auto input_path = data_root_path + path_separator + "input";
    const auto output_path = data_root_path + path_separator + "output";

    auto model = read_model(input_path + path_separator + "model");

#ifdef FILTER_POINTS
    filter_points(model, input_path + path_separator + "model");
#endif

    const auto rcs_file = rcs_data(input_path + path_separator + "rcs.mat");
    const auto rcs = rcs_file.rcs();

    const auto input_image_path = input_path + path_separator + "audi_40";

#ifdef SLICE_POINTS
    color_slices(model, rcs, input_image_path,
                 output_path + path_separator + "colored_slices");
#endif
#ifdef DRAW_LINES
    draw_lines(model, rcs, input_image_path, output_path + path_separator + "rcs_lines");
#endif
#ifdef SUM_PYRAMIDS
    sum_pyramids(model, rcs, input_image_path, output_path + path_separator + "rcs_sums");
#endif
    return EXIT_SUCCESS;
}
