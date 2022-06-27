#include "point_scoring.h"

#include "utils/chronometer.h"

#include "observer_provider.h"
#include "point_cloud_provider.h"
#include "default_options.h"

/**
 * Filters points in a range of scores from a defined minimum (-20dB) to maximum (5dB)
 */
static inline shared_ptr<vector<ScoredPoint>> filter_points(const vector<ScoredPoint>& camera_points,
                                                            const ScoreRange& range) {
    auto time = start_time();
    auto filtered_points = make_shared<vector<ScoredPoint>>();
    for (const auto& point: camera_points) {
        if (point.score_to_dB() < range.min || point.score_to_dB() > range.max) {
            continue;
        }
        filtered_points->push_back(point);
    }
    log_and_start_next(time, "Filtered " + std::to_string(camera_points.size())
                             + " down to " + std::to_string(filtered_points->size()) + " points");
    return filtered_points;
}

static inline vector<ScoredPoint> value_observed_points(
        const vector<observed_point>& observed_points,
        const AbstractDataSet& data_for_observer,
        const observed_factor_func& factor_func
) {
    auto time = start_time();
    auto result = map_vec<observed_point, ScoredPoint, true>(
            observed_points,
            [&data_for_observer, &factor_func](const observed_point& point) {
                double value = data_for_observer.map_to_nearest(point);
                if (abs(point.vertical_angle) > 2.5 || std::isnan(value)) {
                    return ScoredPoint();
                }

                double factor = factor_func(point);
                double final_value = factor * value;
                return ScoredPoint(point.position, point.id, final_value);
            });

    log_and_start_next(time, "Scored " + std::to_string(observed_points.size()) + " points");
    return result;
}

shared_ptr<ScoredCloudPayload> score_points(
        const shared_ptr<InputDataCollector>& inputs,
        const shared_ptr<AbstractDataCollection>& data,
        const observed_factor_func& factor_func,
        const ScoreRange& range_limits,
        const global_colormap_func& colormap_func) {
    const CameraCorrectionParams observer_options{
            .pitch = -2.0,
            .default_height = DEFAULT_HEIGHT,
    };
    const auto observer_provider = make_shared<ObserverProvider>(*inputs, CAMERA_DISTANCE, observer_options);
    const auto point_provider = make_shared<PointCloudProvider>(*inputs);

    const auto observers = observer_provider->observers();
    const shared_ptr<vector<ScoredPoint>> base_points = point_provider->generate_homogenous_cloud();

    vector<ScoredCloud> complete_payload = map_vec<Observer, ScoredCloud, false>(
            observers,
            [&base_points, &data, &range_limits, &factor_func](const Observer& observer) {
                const auto observed_points = observer.observe_points(*base_points);
                const auto data_for_observer = data->at_position(observer.position());
                const auto scored_points = value_observed_points(*observed_points, *data_for_observer, factor_func);
                const auto filtered_points = filter_points(scored_points, range_limits);
                return ScoredCloud(observer, filtered_points);
            });

    return make_shared<ScoredCloudPayload>(ScoredCloudPayload{
            .point_clouds = complete_payload,
            .colormap = colormap_func,
    });
}

static double constexpr identity_factor(const observed_point& point) {
    return std::max(0., 1 - (point.vertical_angle * point.vertical_angle) / 90.);
}

shared_ptr<ScoredCloudPayload> score_points(
        const shared_ptr<InputDataCollector>& inputs,
        const shared_ptr<AbstractDataCollection>& data,
        const ScoreRange& range_limits,
        const global_colormap_func& colormap_func) {
    return score_points(inputs, data, identity_factor, range_limits, colormap_func);
}

