#include "utils.h"
#include "scored_point.h"
#include "colors.h"

typedef Eigen::Hyperplane<double, 4> plane;

void sum_pyramids(const model_ptr& model,
                  const vector<double>& rcs,
                  const string& output_path) {
    /*auto images = get_images(*model);
    auto image_count = images.size();

    for (size_t i = 0; i < image_count; i++) {
        auto image = images[i];
        auto rcs_value = rcs[i];

        auto direction = image.ViewingDirection();
    }*/
}