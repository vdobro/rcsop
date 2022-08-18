#include "point_scoring.h"

#include "utils/chronometer.h"
#include "utils/gauss.h"
#include "utils/mapping.h"
#include "utils/rcs.h"

#include "observer.h"
#include "observer_provider.h"
#include "point_cloud_provider.h"
#include "data_point_projector.h"

namespace rcsop::launcher::utils {
    using rcsop::common::utils::time::start_time;
    using rcsop::common::utils::time::log_and_start_next;
    using rcsop::common::utils::rcs::raw_rcs_to_dB;
    using rcsop::common::utils::rcs::rcs_value_t;

    using rcsop::common::utils::logging::construct_log_prefix;

    using rcsop::common::utils::map_vec_shared;
    using rcsop::common::utils::map_vec;

    using rcsop::common::ScoredPoint;
    using rcsop::common::Observer;

    using rcsop::data::AbstractDataSet;
    using rcsop::data::PointCloudProvider;
    using rcsop::data::ObserverProvider;
    using rcsop::data::DataPointProjector;

    using rcsop::common::coloring::global_colormap_func;

    using rcsop::launcher::utils::multiple_scored_cloud_payload;
    using rcsop::launcher::utils::task_options;

    using dB_range_filter = function<bool(const double)>;

    static auto get_range_filter(const ScoreRange range_limits) -> dB_range_filter {
        return [range_limits](const double dB_value) {
            return range_limits.min <= dB_value;// && dB_value <= range_limits.max;
        };
    }

    /**
     * Filters points in a range of scores from a defined minimum (-20dB) to maximum (5dB)
     */
    static inline shared_ptr<vector<ScoredPoint>> filter_points(
            const vector<ScoredPoint>& camera_points,
            const dB_range_filter& dB_filter) {
        auto filtered_points = make_shared<vector<ScoredPoint>>();
        std::copy_if(camera_points.cbegin(), camera_points.cend(),
                     std::back_inserter(*filtered_points),
                     [&dB_filter](const ScoredPoint& point) -> bool {
                         const auto score = point.score_to_dB();
                         return dB_filter(score);
                     });
        return filtered_points;
    }

    static inline auto value_observed_points(
            const vector<observed_point>& observed_points,
            const AbstractDataSet& data_for_observer,
            const observed_factor_func& factor_func
    ) -> shared_ptr<vector<ScoredPoint>> {
        return map_vec_shared<observed_point, ScoredPoint, true>(
                observed_points,
                [&data_for_observer, &factor_func](const observed_point& point) {
                    double value = data_for_observer.map_to_nearest(point);
                    if (abs(point.vertical_angle) > 5.0 || std::isnan(value)) {
                        return ScoredPoint(point.position, point.id, 0);
                    }

                    double factor = factor_func(point);
                    double final_value = factor * value;
                    return ScoredPoint(point.position, point.id, final_value);
                });
    }

    auto score_points(
            const InputDataCollector& inputs,
            const AbstractDataCollection& data,
            const task_options& task_options,
            const global_colormap_func& colormap_func,
            const observed_factor_func& factor_func) -> shared_ptr<multiple_scored_cloud_payload> {
        const auto dB_range = task_options.db_range;
        const auto observer_provider = make_shared<ObserverProvider>(inputs, task_options.camera);
        const auto projector = make_shared<DataPointProjector>();

        const auto observers = observer_provider->observers_with_positions();
        const auto observer_count = observers.size();

        const auto total_time = start_time();
        const auto range_filter = get_range_filter(dB_range);
        const auto data_filter = [&range_filter](const data::rcs_value_t& rcs_value) -> bool {
            const auto db_value = raw_rcs_to_dB(rcs_value);
            return range_filter(db_value);
        };
        size_t total_count{0};
        size_t filtered_count{0};
        const vector<ScoredCloud> complete_payload = map_vec<Observer, ScoredCloud, false>(
                observers,
                [&data, &projector, &data_filter, &range_filter, &factor_func,
                        &observer_count, &total_count, &filtered_count]
                        (const size_t index, const Observer& observer) {
                    auto time = start_time();
                    const auto data_for_observer = data.get_for_exact_position(observer);
                    const auto projected_points = projector->project_data(data_for_observer, data_filter, observer);
                    const auto observed_points = observer.observe_points(*projected_points);
                    const auto scored_points = value_observed_points(*observed_points, *data_for_observer, factor_func);
                    const auto filtered_points = filter_points(*scored_points, range_filter);

                    log_and_start_next(time, construct_log_prefix(index + 1, observer_count)
                                             + "Scored and filtered " + std::to_string(filtered_points->size()) +
                                             " points");
                    total_count += scored_points->size();
                    filtered_count += filtered_points->size();
                    return ScoredCloud(observer, filtered_points);
                });
        log_and_start_next(total_time, "Scored a total of " + std::to_string(total_count) +
                                       " and filtered down to " + std::to_string(filtered_count) +
                                       " for a total of " +  std::to_string(observer_count) +
                                       " observers.");

        return make_shared<multiple_scored_cloud_payload>(multiple_scored_cloud_payload{
                .point_clouds = complete_payload,
                .colormap = colormap_func,
        });
    }

    auto identity_factor(const observed_point& point) -> double {
        return 1;
    }
}
