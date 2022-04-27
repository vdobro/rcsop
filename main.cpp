#include <string>

#include "utils.h"
#include "mat_reader.h"

#undef FILTER_POINTS
#define SLICE_POINTS
#define DRAW_LINES
#undef SUM_PYRAMIDS

void color_slices(const shared_ptr<colmap::Reconstruction>& model,
                  const vector<double>& rcs,
                  const string& input_path,
                  const string& output_path);

void draw_lines(const model_ptr& model,
                const vector<double>& rcs,
                const string& output_path);

void sum_pyramids(const model_ptr& model,
                  const vector<double>& rcs,
                  const string& output_path);

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

#ifdef SLICE_POINTS
    color_slices(model, rcs,
                 input_path + path_separator + "audi_40",
                 output_path + path_separator + "colored_slices");
#endif
#ifdef DRAW_LINES
    draw_lines(model, rcs, output_path + path_separator + "rcs_lines");
#endif
#ifdef SUM_PYRAMIDS
    sum_pyramids(model, rcs, output_path + path_separator + "rcs_sums");
#endif
    return EXIT_SUCCESS;
}
