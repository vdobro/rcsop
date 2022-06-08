#include "draw_lines.h"

#include "colors.h"

typedef Eigen::ParametrizedLine<double, 3> model_line;

using sfm::rendering::construct_colormap_function;
using sfm::rendering::map_turbo;

void draw_lines(SparseCloud& model,
                const vector<double>& rcs,
                const path& input_path,
                const path& output_path) {

    auto cameras = model.get_cameras();
    auto camera_count = cameras.size();

    auto rcs_color_map = construct_colormap_function(map_turbo, rcs);

    for (size_t i = 0; i < camera_count; i++) {
        auto image = cameras.at(i);
        auto direction = image.get_direction();
        auto position = image.get_position();

        auto color = rcs_color_map(rcs.at(i));

        int steps = 1000;
        double distance = position.norm();
        double step = distance / steps;;
        double j = 0.0;

        auto line = model_line(position, direction);

        while (j < position.norm()) {
            auto new_point = line.pointAt(j);
            model.add_point(new_point, color);
            j += step;
        }
    }

    model.save(output_path);
}
