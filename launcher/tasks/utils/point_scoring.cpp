#include "point_scoring.h"

#include "observer_provider.h"
#include "point_cloud_provider.h"

/**
 * Filters points in a range of scores from a defined minimum (-20dB) to maximum (5dB)
 */
static shared_ptr<vector<ScoredPoint>> filter_points(const ScoredPointMap& camera_points) {
    shared_ptr<vector<ScoredPoint>> filtered_points = make_shared<vector<ScoredPoint>>();
    for (const auto& point: camera_points) {
        ScoredPoint old_point = point.second;
        if (old_point.score_to_dB() < -20 || old_point.score_to_dB() > 5) {
            continue;
        }
        filtered_points->push_back(old_point);
    }
    return filtered_points;
}

shared_ptr<ScoredCloudPayload> score_points(const shared_ptr<InputDataCollector>& inputs,
                                            const shared_ptr<AbstractDataCollection>& data,
                                            const observed_factor_func& factor_func,
                                            const local_colormap_func& colormap_func) {
    const auto observer_provider = make_shared<ObserverProvider>(*inputs);
    const auto point_provider = make_shared<PointCloudProvider>(*inputs);

    const auto observers = observer_provider->observers();
    const auto base_points = point_provider->get_base_scored_points();

    vector<ScoredPoint> all_points; // ONLY use for colormap calibration
    vector<ScoredCloud> complete_payload;

    for (const auto& observer: observers) {
        ScoredPointMap camera_points(base_points);

        auto observed_points = observer.observe_points(camera_points);
        shared_ptr<AbstractDataSet> data_for_observer = data->at_position(observer.position());

        for (const auto& point: observed_points) {
            if (point.vertical_angle > 5) {
                continue;
            }
            double value = data_for_observer->map_to_nearest(point);
            if (!std::isnan(value)) {
                double factor = factor_func(point);
                double final_value = factor * value;
                camera_points.at(point.id).increment_score(final_value);
            }
        }
        shared_ptr<vector<ScoredPoint>> filtered_points = filter_points(camera_points);
        all_points.insert(all_points.end(), filtered_points->begin(), filtered_points->end());

        complete_payload.emplace_back(observer, filtered_points);
    }

    auto score_range = ScoredPoint::get_score_range(all_points);
    auto colormap = construct_colormap_function(colormap_func, score_range.min, score_range.max);

    return make_shared<ScoredCloudPayload>(ScoredCloudPayload{
            .point_clouds = complete_payload,
            .colormap = colormap,
    });
}

static double constexpr identity_factor(const observed_point& point) {
    return 1;
}

shared_ptr<ScoredCloudPayload> score_points(
        const shared_ptr<InputDataCollector>& inputs,
        const shared_ptr<AbstractDataCollection>& data,
        const local_colormap_func& colormap_func) {
    return score_points(inputs, data, identity_factor, colormap_func);
}

