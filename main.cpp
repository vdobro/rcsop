#include <string>
#include "utils/types.h"

#include "utils/mat_reader.h"
#include "utils/cleanup.h"

#include "examples/draw_lines.h"
#include "examples/slices.h"

#include "rcs_sums.h"
#include "azimuth_angles.h"

#define FILTER_POINTS
#define DRAW_LINES
#define SLICE_POINTS

#define SUM_RCS
#define SUM_AZIMUTH
#define AZIMUTH_ANGLES

int main() {
    const path data_root_path{"data"};
    const path input_path{data_root_path / "input"};
    const path input_image_path{input_path / "audi_40"};

    const path output_path{data_root_path / "output"};

    const path model_path{input_path / "model"};
    auto model = read_model(model_path);

#ifdef FILTER_POINTS
    const path filtered_model_path{input_path / "filtered_model"};
    filter_points(model, filtered_model_path);
#endif

    const path rcs_file_path{input_path / "rcs.mat"};
    const auto rcs_file = rcs_data(rcs_file_path);
    const auto rcs = rcs_file.at_height(40)->rcs();

#ifdef SLICE_POINTS
    const path slice_output_path{output_path / "colored_slices"};
    color_slices(model, rcs, input_image_path, slice_output_path);
#endif
#ifdef DRAW_LINES
    const path line_output_path{output_path / "rcs_lines"};
    draw_lines(model, rcs, input_image_path, line_output_path);
    model = read_model(model_path);
#endif
#ifdef SUM_RCS
    const path accumulated_rcs_output_path{output_path / "rcs_sums"};
    accumulate_rcs(model, rcs_file, input_image_path, accumulated_rcs_output_path);
#endif
#ifdef SUM_AZIMUTH
    const path accumulated_azimuth_output_path{output_path / "azimuth_sums"};
    accumulate_azimuth(model, rcs_file, input_image_path, accumulated_azimuth_output_path);
#endif
#ifdef AZIMUTH_ANGLES
    const path azimuth_angle_data_path{input_path / "AudiAuswertung"};
    const path output_path_azimuth{output_path / "azimuth_angles"};

    auto azimuthdata = display_azimuth(model, input_image_path, azimuth_angle_data_path, output_path_azimuth);
    render_to_files(*azimuthdata, output_path_azimuth);
#endif
    return EXIT_SUCCESS;
}
