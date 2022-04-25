#include "cleanup.h"
#include "utils.h"
#include "slices.h"

const bool FILTER_POINTS = false;

int main() {
    auto path = "input";
    auto model = read_model(path);

    if (FILTER_POINTS) {
        filter_points(std::move(model), "filtered");
    }

    color_slices(std::move(model), "colored_slices");

    return 0;
}
