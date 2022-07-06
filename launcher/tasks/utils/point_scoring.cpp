#include "point_scoring.h"

#include "utils/chronometer.h"

#include "observer_provider.h"
#include "point_cloud_provider.h"

/**
 * Filters points in a range of scores from a defined minimum (-20dB) to maximum (5dB)
 */
static inline shared_ptr<vector<ScoredPoint>> filter_points(
        const vector<ScoredPoint>& camera_points,
        const ScoreRange& range) {
    auto filtered_points = make_shared<vector<ScoredPoint>>();
    std::copy_if(camera_points.cbegin(), camera_points.cend(),
                 std::back_inserter(*filtered_points),
                 [&range](auto& point) {
                     const auto score = point.score_to_dB();
                     return score >= range.min;// && score <= range.max;
                 });
    return filtered_points;
}

static inline shared_ptr<vector<ScoredPoint>> value_observed_points(
        const vector<observed_point>& observed_points,
        const AbstractDataSet& data_for_observer,
        const observed_factor_func& factor_func
) {
    return map_vec_shared<observed_point, ScoredPoint, true>(
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
}

shared_ptr<ScoredCloudPayload> score_points(
        const InputDataCollector& inputs,
        const AbstractDataCollection& data,
        const task_options& task_options,
        const global_colormap_func& colormap_func,
        const observed_factor_func& factor_func) {
    const CameraCorrectionParams observer_options{
            .pitch = -5.4,
    };
    const auto distance_to_origin = task_options.camera_distance_to_origin;
    const auto range_limits = task_options.db_range;
    const auto observer_provider = make_shared<ObserverProvider>(inputs, distance_to_origin, observer_options);
    const auto point_provider = make_shared<PointCloudProvider>(inputs, distance_to_origin);

    const auto observers = observer_provider->observers_with_positions();
    const auto observer_count = observers.size();
    const shared_ptr<vector<ScoredPoint>> base_points = point_provider
            ->generate_homogenous_cloud(30);

    const auto total_time = start_time();
    const vector<ScoredCloud> complete_payload = map_vec<Observer, ScoredCloud, false>(
            observers,
            [&base_points, &data, &range_limits, &factor_func, &observer_count]
                    (const size_t index, const Observer& observer) {
                auto time = start_time();

                const auto observed_points = observer.observe_points(*base_points);
                const auto data_for_observer = data.get_for_exact_position(observer);
                const auto scored_points = value_observed_points(*observed_points, *data_for_observer, factor_func);
                const auto filtered_points = filter_points(*scored_points, range_limits);

                log_and_start_next(time, construct_log_prefix(index + 1, observer_count)
                                         + "Scored and filtered " + std::to_string(filtered_points->size()) +
                                         " points");
                return ScoredCloud(observer, filtered_points);
            });
    log_and_start_next(total_time, "Scored and filtered " + std::to_string(base_points->size()) +
                                   " for each of " + std::to_string(observer_count) + " observers_with_positions");

    return make_shared<ScoredCloudPayload>(ScoredCloudPayload{
            .point_clouds = complete_payload,
            .colormap = colormap_func,
    });
}

double identity_factor(const observed_point& point) {
    return 1;
}
