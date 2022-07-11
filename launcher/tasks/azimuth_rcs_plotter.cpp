#include "azimuth_rcs_plotter.h"

#include "utils/mapping.h"
#include "utils/logging.h"
#include "utils/gauss.h"
#include "utils/point_scoring.h"

#include "colors.h"
#include "observer_renderer.h"
#include "scored_cloud.h"
#include "azimuth_minimap_provider.h"

namespace rcsop::launcher::tasks {
    using rcsop::common::utils::gauss::rcs_gaussian_vertical;
    using rcsop::common::utils::points::Vector2d;
    using rcsop::common::utils::map_vec;

    using rcsop::common::ScoreRange;
    using rcsop::common::ScoredCloud;
    using rcsop::common::Texture;

    using rcsop::data::AZIMUTH_RCS_MAT;
    using rcsop::data::AZIMUTH_RCS_MINIMAP;

    using rcsop::rendering::texture_rendering_options;
    using rcsop::rendering::ObserverRenderer;
    using rcsop::rendering::coloring::construct_colormap_function;

    void azimuth_rcs_plotter(const InputDataCollector& inputs,
                             const task_options& options) {
            const auto azimuth_data = inputs.data<AZIMUTH_RCS_MAT>(false);
            azimuth_data->use_filtered_peaks();

            const ScoreRange range = options.db_range;
            const auto color_map = construct_colormap_function(options.rendering.color_map, range);
            const auto scored_payload = score_points(inputs, *azimuth_data, options, color_map, rcs_gaussian_vertical);
            const texture_rendering_options minimap_position = {
                    .coordinates= Vector2d(915., 420.),
                    .size = Vector2d(400., 300.),
            };

            const auto& point_clouds = scored_payload->point_clouds;
            const auto minimaps = inputs.data<AZIMUTH_RCS_MINIMAP>(false);
            auto renderers = map_vec<ScoredCloud, ObserverRenderer, true>(
                    point_clouds,
                    [&color_map, &options, &minimaps, &minimap_position]
                            (const ScoredCloud& scored_cloud) {
                        ObserverRenderer renderer(scored_cloud, color_map, options.rendering);
                        Texture minimap = minimaps->for_position(scored_cloud.observer());

                        renderer.add_texture(minimap, minimap_position);
                        return renderer;
                    });
            const auto heights = scored_payload->observer_heights();
            batch_render(renderers, options, heights);
    }
}
