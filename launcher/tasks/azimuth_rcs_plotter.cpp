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

    auto payload_size = points.size();
    for (size_t index = 1; index <= payload_size; index++) {
        ScoredCloud scored_cloud = points[index - 1];
        ObserverRenderer renderer(scored_cloud);
        renderer.render(output_path, colormap, construct_log_prefix(index, payload_size));
    }
}
