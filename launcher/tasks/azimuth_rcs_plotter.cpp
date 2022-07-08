#include "azimuth_rcs_plotter.h"

#include "utils/logging.h"
#include "utils/gauss.h"
#include "utils/point_scoring.h"

#include "colors.h"
#include "observer_renderer.h"
#include "scored_cloud.h"
#include "azimuth_minimap_provider.h"

using namespace sfm::rendering;

void azimuth_rcs_plotter(const InputDataCollector& inputs,
                         const task_options& options) {
    const auto azimuth_data = inputs.data<AZIMUTH_RCS_MAT>(false);
    azimuth_data->use_filtered_peaks();

    const ScoreRange range = options.db_range;
    const auto color_map = construct_colormap_function(options.rendering.color_map, range);
    const auto scored_payload = score_points(inputs, *azimuth_data, options, color_map, rcs_gaussian_vertical);
    const auto& points = scored_payload->point_clouds;
    const auto heights = scored_payload->observer_heights();
    map<height_t, path> height_folders;
    for (auto height: scored_payload->observer_heights()) {
        path height_path{options.output_path / (std::to_string(height) + "cm")};
        std::filesystem::create_directories(height_path);
        height_folders.insert(make_pair(height, height_path));
    }

    const texture_rendering_options minimap_position = {
            .coordinates= Vector2d(915., 420.),
            .size = Vector2d(400., 300.),
    };
    const auto minimaps = inputs.data<AZIMUTH_RCS_MINIMAP>(false);
    auto payload_size = points.size();
    for (size_t index = 1; index <= payload_size; index++) {
        ScoredCloud scored_cloud = points[index - 1];
        ObserverRenderer renderer(scored_cloud, color_map, options.rendering);

        Texture minimap = minimaps->for_position(scored_cloud.observer());
        renderer.add_texture(minimap, minimap_position);

        path output_path = scored_cloud.observer().has_position()
                ? height_folders.at(scored_cloud.observer().position().height)
                : options.output_path;
        renderer.render(output_path, construct_log_prefix(index, payload_size));
    }
}
