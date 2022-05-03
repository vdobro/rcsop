#include <Eigen/Geometry>

#include "utils.h"
#include "colors.h"

void draw_lines(const model_ptr& model,
                const vector<double>& rcs,
                const string& output_path) {
    auto images = get_images(*model);
    auto image_count = images.size();

    auto rcs_colors = color_values(rcs, map_turbo);

    for (size_t i = 0; i < image_count; i++) {
        auto image = images[i];
        auto direction = image.ViewingDirection();
        auto position = get_image_position(image);

        auto color = rcs_colors[i];

        int steps = 1000;
        double distance = position.norm();
        double step = distance / steps;;
        double j = 0.0;

        auto line = model_line(position, direction);

        while (j < position.norm()) {
            auto new_point = line.pointAt(j);
            model->AddPoint3D(new_point, colmap::Track(), color);
            j += step;
        }
    }
    write_model(model, output_path);
}
