#include "azimuth_rcs_plotter.h"

#include "utils/mapping.h"
#include "utils/gauss.h"
#include "utils/point_scoring.h"

#include "colors.h"
#include "output_data_writer.h"
#include "model_writer.h"
#include "observer_renderer.h"
#include "scored_cloud.h"
#include "azimuth_minimap_provider.h"

namespace rcsop::launcher::tasks {
    using rcsop::common::utils::gauss::rcs_gaussian_vertical;
    using rcsop::common::utils::points::vec2;
    using rcsop::common::utils::map_vec;

    using rcsop::common::ScoreRange;
    using rcsop::common::ScoredCloud;
    using rcsop::common::Texture;
    using rcsop::common::OutputDataWriter;
    using rcsop::common::scored_cloud_payload;

    using rcsop::data::AZIMUTH_RCS_MAT;
    using rcsop::data::AZIMUTH_RCS_MINIMAP;
    using rcsop::data::ModelWriter;

    using rcsop::rendering::texture_rendering_options;
    using rcsop::rendering::ObserverRenderer;
    using rcsop::common::coloring::construct_colormap_function;

    void azimuth_rcs_plotter(const InputDataCollector& inputs,
                             const task_options& options) {
        auto azimuth_data = inputs.data<AZIMUTH_RCS_MAT, false>();

        azimuth_data->use_filtered_peaks();

        ScoreRange range = options.db_range;
        auto color_map = construct_colormap_function(options.rendering.color_map, range);
        auto scored_payload = score_points(inputs, *azimuth_data, options, color_map, rcs_gaussian_vertical);
        texture_rendering_options minimap_position = {
                .coordinates= vec2(915., 420.),
                .size = vec2(400., 300.),
        };

        auto& point_clouds = scored_payload->point_clouds;
        auto minimaps = inputs.data<AZIMUTH_RCS_MINIMAP>();
        auto renderers = map_vec<ScoredCloud, shared_ptr<OutputDataWriter>, true>(
                point_clouds,
                [&color_map, &options, &minimaps, &minimap_position]
                        (const ScoredCloud& scored_cloud) -> shared_ptr<OutputDataWriter> {
                    auto renderer = make_shared<ObserverRenderer>(scored_cloud, color_map, options.rendering);
                    Texture minimap = minimaps->for_position(scored_cloud.observer());

                    renderer->add_texture(minimap, minimap_position);
                    return renderer;
                });
        auto point_clouds_exploded = scored_payload->extract_single_payloads();
        auto model_writers = map_vec<scored_cloud_payload, shared_ptr<OutputDataWriter>, true>(
                point_clouds_exploded,
                [&inputs, color_map](const scored_cloud_payload& payload) -> shared_ptr<OutputDataWriter> {
                    auto model_writer = inputs.get_model_writer();
                    auto observer = payload.point_cloud.observer();
                    model_writer->set_observer_position(observer.position(), observer.native_camera());
                    model_writer->add_points(payload);
                    return model_writer;
                });

        auto heights = scored_payload->observer_heights();

        batch_output(renderers, options, heights);
        batch_output(model_writers, options, heights);
    }
}
