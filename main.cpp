#include "cleanup.h"
#include "utils.h"
#include "slices.h"

#include "read_rcs.h"

const bool FILTER_POINTS = false;

int main() {
    auto path = "data/input";
    auto model = read_model(path);

    if (FILTER_POINTS) {
        filter_points(std::move(model), "data/filtered");
    }

    color_slices(std::move(model), "data/colored_slices");

    auto matfile = open_mat_file("data/rcs.mat");
    auto table = get_table(matfile);
    auto index = get_row_for_height(40, table);

    auto rcs = get_rcs(index, table);
    auto rcs_dbs = get_rcs_db(index, table);
    auto angles = get_angles(index, table);
    auto ranges = get_ranges(index, table);

    close_mat_file(matfile);

    return EXIT_SUCCESS;
}
