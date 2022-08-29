#include "azimuth_rcs_plotter.h"

#include "utils/types.h"
#include "utils/mapping.h"
#include "utils/gauss.h"
#include "utils/point_scoring.h"

#include "colors.h"
#include "observer.h"
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
    using rcsop::common::multiple_scored_cloud_payload;
    using rcsop::common::scored_cloud_payload;

    using rcsop::data::AZIMUTH_RCS_MAT;
    using rcsop::data::AZIMUTH_RCS_MINIMAP;
    using rcsop::data::AzimuthRcsDataCollection;
    using rcsop::data::AbstractDataCollection;
    using rcsop::data::AzimuthMinimapProvider;
    using rcsop::data::ModelWriter;

    using rcsop::rendering::texture_rendering_options;
    using rcsop::rendering::ObserverRenderer;
    using rcsop::common::coloring::construct_colormap_function;
    using rcsop::common::data_observer_translation;
    using rcsop::common::global_colormap_func;

    using rcsop::launcher::utils::data_with_observer_options;

    using std::clog;
    using std::endl;

    static const regex data_label_pattern("^(\\d{1,3})(°)?$");

    static auto translate_label_to_translation(const string& label) -> data_observer_translation {
        smatch label_match;
        if (!regex_match(label, label_match, data_label_pattern)) {
            std::clog << "Warning: RCS data folder doesn't match expected name pattern, using default rotation value."
                      << std::endl;
            return {};
        }
        long roll_degrees = std::stoi(label_match[1]);
        data_observer_translation observer_translation{
                .roll = static_cast<double>(roll_degrees),
        };
        return observer_translation;
    }

    static auto map_labeled_data(const map<string, shared_ptr<AzimuthRcsDataCollection>>& data) {
        vector<data_with_observer_options> result;
        for (auto& [label, data_set]: data) {
            auto observer_translation = translate_label_to_translation(label);
            result.push_back(
                    {
                            .observer_options = observer_translation,
                            .data_collection = data_set
                    });
        }
        return result;
    }

    static void plot_to_images(const multiple_scored_cloud_payload& payload,
                               const AzimuthMinimapProvider& minimaps,
                               const global_colormap_func& color_map,
                               const task_options& options) {
        texture_rendering_options minimap_position = {
                .coordinates = vec2(915., 420.),
                .size = vec2(400., 300.),
        };

        auto& point_clouds = payload.point_clouds;
        auto renderers = map_vec<ScoredCloud, shared_ptr<OutputDataWriter>>(
                point_clouds,
                [&color_map, &options, &minimaps, &minimap_position]
                        (const ScoredCloud& scored_cloud) -> shared_ptr<OutputDataWriter> {
                    auto renderer = make_shared<ObserverRenderer>(scored_cloud, color_map, options.rendering);
                    const auto& minimap = minimaps.for_position(scored_cloud.observer());

                    renderer->add_texture(minimap, minimap_position);
                    return renderer;
                });
        auto heights = payload.observer_heights();
        batch_output(renderers, options, heights);
    }

    static void plot_to_models(const multiple_scored_cloud_payload& payload,
                               const InputDataCollector& inputs,
                               const global_colormap_func& color_map,
                               const task_options& options) {
        auto point_clouds_exploded = payload.extract_single_payloads();
        auto model_writers = map_vec<scored_cloud_payload, shared_ptr<OutputDataWriter>, true>(
                point_clouds_exploded,
                [&inputs, color_map](const scored_cloud_payload& payload) -> shared_ptr<OutputDataWriter> {
                    auto model_writer = inputs.get_model_writer();
                    auto observer = payload.point_cloud.observer();
                    model_writer->set_observer_position(observer.position(), observer.native_camera());
                    model_writer->add_points(payload);
                    return model_writer;
                });

        auto heights = payload.observer_heights();
        batch_output(model_writers, options, heights);
    }

    void azimuth_rcs_plotter(const InputDataCollector& inputs,
                             const task_options& options) {
        ScoreRange range = options.db_range;
        auto color_map = construct_colormap_function(options.rendering.color_map, range);

        auto azimuth_data = inputs.data<AZIMUTH_RCS_MAT, true>();
        auto minimaps = inputs.data<AZIMUTH_RCS_MINIMAP, false>();

        auto data_with_translation = map_labeled_data(azimuth_data);
        auto scored_payload = score_points(inputs, data_with_translation, options, color_map, rcs_gaussian_vertical);

        if (options.prefilter_data) {
            for (auto& [_, data]: azimuth_data) {
                data->use_filtered_peaks();
            }
        }

        auto scored_filtered_payload = score_points(inputs, data_with_translation, options, color_map, rcs_gaussian_vertical);

        clog << endl <<  "Rendering to sparse cloud models:" << endl;
        plot_to_models(*scored_payload, inputs, color_map, options);

        clog << endl << "Rendering to images:" << endl;
        plot_to_images(*scored_filtered_payload, *minimaps, color_map, options);
    }
}
