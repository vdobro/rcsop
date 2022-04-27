#include <string>

#include "utils.h"
#include "mat_reader.h"

#undef FILTER_POINTS
#define SLICE_POINTS
#undef DRAW_LINES
#undef SUM_PYRAMIDS

void color_slices(const shared_ptr<colmap::Reconstruction>& model,
                  const vector<double>& rcs,
                  const string& output_path);

void draw_lines(const model_ptr& model,
                const vector<double>& rcs,
                const string& output_path);

void sum_pyramids(const model_ptr& model,
                  const vector<double>& rcs,
                  const string& output_path);

int main() {
    const string data_root_path = "data";
    auto path = data_root_path + "/input";

    auto model = read_model(path);

#ifdef FILTER_POINTS
    filter_points(model, data_root_path + "/input");
#endif

    auto rcs_file = rcs_data(data_root_path + "/rcs.mat");
    auto rcs = rcs_file.rcs();

#ifdef SLICE_POINTS
    color_slices(model, rcs, data_root_path + "/colored_slices");
#endif
#ifdef DRAW_LINES
    draw_lines(model, rcs, data_root_path + "/rcs_lines");
#endif
#ifdef SUM_PYRAMIDS
    sum_pyramids(model, rcs, data_root_path + "/rcs_sums");
#endif
    return EXIT_SUCCESS;
}
