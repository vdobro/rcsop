#include "azimuth_rcs_plotter.h"

#include "utils/logging.h"
#include "utils/point_scoring.h"

#include "colors.h"
#include "options.h"
#include "observer_renderer.h"
#include "scored_cloud.h"
#include "azimuth_minimap_provider.h"

using namespace sfm::rendering;

void azimuth_rcs_plotter(const shared_ptr<InputDataCollector>& inputs,
                         const path& output_path) {
    const auto azimuth_data = inputs->data<AZIMUTH_RCS_MAT>(false);
    const auto scored_payload = score_points(inputs, azimuth_data, COLOR_MAP);
    const auto& points = scored_payload->point_clouds;
    const auto& colormap = scored_payload->colormap;

    const TextureRenderParams minimap_position = {
            .coordinates= Vector2d(915., 420.),
            .size = Vector2d(400., 300.),
    };
    const auto minimaps = inputs->data<AZIMUTH_RCS_MINIMAP>(false);
    auto payload_size = points.size();
    for (size_t index = 1; index <= payload_size; index++) {
        ScoredCloud scored_cloud = points[index - 1];
        ObserverRenderer renderer(scored_cloud);

        shared_ptr<Texture> minimap = minimaps->at_position(scored_cloud.observer().position());
        renderer.add_texture(*minimap, minimap_position);
        renderer.render(output_path, colormap, construct_log_prefix(index, payload_size));
    }
}
