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
    auto filtered_points = make_shared<vector<ScoredPoint>>();
    std::copy_if(camera_points.cbegin(), camera_points.cend(),
                 std::back_inserter(*filtered_points),
                 [&range](auto& point) {
        const auto score = point.score_to_dB();
        return score >= range.min && score <= range.max;
    });
    return filtered_points;
}

static inline vector<ScoredPoint> value_observed_points(
        const vector<observed_point>& observed_points,
        const AbstractDataSet& data_for_observer,
        const observed_factor_func& factor_func
) {
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
    const auto observer_count = observers.size();
    const shared_ptr<vector<ScoredPoint>> base_points = point_provider->generate_homogenous_cloud();

    const auto total_time = start_time();
    const vector<ScoredCloud> complete_payload = map_vec<Observer, ScoredCloud, true>(
            observers,
            [&base_points, &data, &range_limits, &factor_func]
                    (const size_t index, const Observer& observer) {

                const auto observed_points = observer.observe_points(*base_points);
                const auto data_for_observer = data->at_position(observer.position());
                const auto scored_points = value_observed_points(*observed_points, *data_for_observer, factor_func);
                const auto filtered_points = filter_points(scored_points, range_limits);
                return ScoredCloud(observer, filtered_points);
            });
    log_and_start_next(total_time, "Scored and filtered " + std::to_string(base_points->size()) +
                             " for each of " + std::to_string(observer_count) + " observers");

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

