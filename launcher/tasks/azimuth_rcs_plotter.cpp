#include "azimuth_rcs_plotter.h"

#include "utils/logging.h"

#include "point_cloud_provider.h"
#include "observer_provider.h"

#include "colors.h"
#include "options.h"
#include "observer_renderer.h"
#include "scored_cloud.h"
#include "azimuth_minimap_provider.h"

/**
 * Filters points in a range of scores from a defined minimum (-20dB) to maximum (5dB)
 */
static vector<ScoredPoint> filter_points(const ScoredPointMap& camera_points) {
    vector<ScoredPoint> filtered_points;
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
    shared_ptr<AzimuthMinimapProvider> minimaps = inputs->data<AZIMUTH_RCS_MINIMAP>(false);

    vector<ScoredPoint> all_points; // ONLY use for colormap calibration
    vector<ScoredCloud> complete_payload;
    for (const auto& observer: observers) {
        ScoredPointMap camera_points(base_points);

        auto observed_points = observer.observe_points(camera_points);
        auto data = rcs->at_position(observer.position());

        for (const auto& point: observed_points) {
            if (point.vertical_angle > 5) {
                continue;
            }
            auto value = data->find_nearest(point.distance_in_world, point.horizontal_angle);
            if (!std::isnan(value)) {
                camera_points.at(point.id).increment_score(value);
            }
        }
        shared_ptr<vector<ScoredPoint>> filtered_points = make_shared<vector<ScoredPoint>>(filter_points(camera_points));
        all_points.insert(all_points.end(), filtered_points->begin(), filtered_points->end());

        complete_payload.emplace_back(observer, filtered_points);
    }

    auto score_range = ScoredPoint::get_score_range(all_points);
    auto colormap = construct_colormap_function(COLOR_MAP, score_range.min, score_range.max);

    auto payload_count = complete_payload.size();
    for (size_t index = 1; index <= payload_count; index++) {
        ScoredCloud scored_cloud = complete_payload[index - 1];
        ObserverRenderer renderer(scored_cloud);
        renderer.render(output_path, colormap, construct_log_prefix(index, payload_count));
    }
}
