#include <string>
using std::string;

#include "utils.h"
#include "cleanup.h"
#include "slices.h"
#include "mat_reader.h"

const bool FILTER_POINTS = false;

int main() {
    const string data_root_path = "data";
    auto path = data_root_path + "/input";

    auto model = read_model(path);

    if (FILTER_POINTS) {
        filter_points(std::move(model), "data/filtered");
    }


    auto rcs_file = open_mat_file(data_root_path + "/rcs.mat");
    auto table = get_table(rcs_file);
    auto index = get_row_for_height(40, table);

    auto rcs = get_rcs(index, table);
    color_slices(std::move(model), rcs, data_root_path + "/colored_slices");

    auto rcs_dbs = get_rcs_db(index, table);
    auto angles = get_angles(index, table);
    auto ranges = get_ranges(index, table);

    close_mat_file(rcs_file);

    return EXIT_SUCCESS;
}
