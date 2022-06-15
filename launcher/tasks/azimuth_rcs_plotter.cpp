#include "azimuth_rcs_plotter.h"

#include "point_cloud_provider.h"
#include "observer_provider.h"

#include "colors.h"
#include "options.h"
#include "observer_renderer.h"
#include "scored_cloud.h"
#include "utils/logging.h"

/**
 * Filters points in a range of scores from a defined minimum (-20dB) to maximum (5dB)
 */
static vector<scored_point> filter_points(const scored_point_map& camera_points) {
    vector<scored_point> filtered_points;
    for (const auto& point: camera_points) {
        auto old_point = point.second;
        if (old_point.score_to_dB() < -20 || old_point.score_to_dB() > 5) {
            continue;
        }
        filtered_points.push_back(old_point);
    }
    return filtered_points;
}

using namespace sfm::rendering;

void azimuth_rcs_plotter(const shared_ptr<InputDataCollector>& inputs,
                         const path& output_path) {
    const auto observer_provider = make_shared<ObserverProvider>(*inputs);
    const auto point_provider = make_shared<PointCloudProvider>(*inputs);

    const auto observers = observer_provider->observers();
    const auto base_points = point_provider->get_base_scored_points();
    shared_ptr<AzimuthRcsMap> rcs = inputs->data<AZIMUTH_RCS_MAT>(false);

    vector<scored_point> all_points; // ONLY use for colormap calibration
    vector<ScoredCloud> complete_payload;
    for (const auto& observer: observers) {
        scored_point_map camera_points(base_points);

        auto observed_points = observer.observe_points(camera_points);
        auto data = rcs->at_position(observer.position());

        for (const auto& point: observed_points) {
            if (point.horizontal_angle > 5) {
                continue;
            }
            auto value = data->find_nearest(point.distance_in_world, point.horizontal_angle);
            if (!std::isnan(value)) {
                camera_points.at(point.id).increment_score(value);
            }
        }
        vector<scored_point> filtered_points = filter_points(camera_points);
        all_points.insert(all_points.end(), filtered_points.begin(), filtered_points.end());

        ScoredCloud payload(observer,filtered_points);
        complete_payload.push_back(payload);
    }

    auto score_range = scored_point::get_score_range(all_points);
    auto colormap = construct_colormap_function(COLOR_MAP, score_range.min, score_range.max);

    size_t index = 1;
    auto payload_count = complete_payload.size();
    for (const auto& payload: complete_payload) {
        ObserverRenderer renderer(payload);
        renderer.render(output_path, colormap,
                        construct_log_prefix(index++, payload_count));
    }
}
