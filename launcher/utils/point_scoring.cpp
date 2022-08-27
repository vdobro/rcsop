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
    using rcsop::common::utils::filter_vec_shared;

    using rcsop::common::ScoredPoint;
    using rcsop::common::Observer;

    using rcsop::data::AbstractDataSet;
    using rcsop::data::PointCloudProvider;
    using rcsop::data::ObserverProvider;
    using rcsop::data::DataPointProjector;

    using rcsop::common::coloring::global_colormap_func;

    using rcsop::launcher::utils::multiple_scored_cloud_payload;
    using rcsop::launcher::utils::task_options;

    using dB_range_filter = function<bool(double)>;

    static auto get_range_filter(const ScoreRange range_limits) -> dB_range_filter {
        return [range_limits](const double dB_value) {
            return range_limits.min <= dB_value;// && dB_value <= range_limits.max;
        };
    }

    /**
     * Filters points in a range of scores from a defined minimum (-20dB) to maximum (5dB)
     */
    static inline auto filter_points(
            const vector<ScoredPoint>& camera_points,
            const dB_range_filter& dB_filter) -> shared_ptr <vector<ScoredPoint>> {
        return filter_vec_shared<ScoredPoint>(
                camera_points,
                [&dB_filter](const ScoredPoint& point) -> bool {
                    const auto score = point.score_to_dB();
                    return dB_filter(score);
                });
    }

    auto score_points(
            const InputDataCollector& inputs,
            const vector<data_with_observer_options>& labeled_data,
            const task_options& task_options,
            const global_colormap_func& color_map_func,
            const observed_factor_func& factor_func) -> shared_ptr<multiple_scored_cloud_payload const> {
        auto dB_range = task_options.db_range;
        auto observer_provider = make_shared<ObserverProvider>(inputs, task_options.camera, true);
        auto projector = make_shared<DataPointProjector>();

        auto observers = observer_provider->observers_with_positions();
        auto observer_count = observers.size();

        auto total_time = start_time();
        auto range_filter = get_range_filter(dB_range);

        size_t total_count{0};
        size_t filtered_count{0};
        const vector<ScoredCloud> complete_payload = map_vec<Observer, ScoredCloud>(
                observers,
                [&labeled_data, &projector, range_filter, &factor_func,
                        &observer_count, &total_count, &filtered_count]
                        (const size_t index, const Observer& observer) {
                    auto time = start_time();
                    auto relevant_points = make_shared<vector<ScoredPoint>> ();
                    for (const auto& [observer_options, data_collection]: labeled_data) {
                        auto data_for_observer = data_collection->get_for_exact_position(observer);
                        auto observer_with_translation = observer.clone_with_data(observer_options);
                        auto projected_points = projector->project_data(
                                data_for_observer, range_filter, factor_func, observer_with_translation);
                        total_count += projected_points->size();

                        auto filtered_points = filter_points(*projected_points, range_filter);
                        filtered_count += filtered_points->size();

                        relevant_points->insert(relevant_points->end(),
                                                filtered_points->cbegin(), filtered_points->cend());
                    }

                    log_and_start_next(time, construct_log_prefix(index + 1, observer_count)
                                             + "Scored and filtered " + std::to_string(relevant_points->size()) +
                                             " points at " + observer.position().str());
                    return ScoredCloud(observer, relevant_points);
                });
        log_and_start_next(total_time, "Scored a total of " + std::to_string(total_count) +
                                       " and filtered down to " + std::to_string(filtered_count) +
                                       " for a total of " + std::to_string(observer_count) +
                                       " observers.");

        return make_shared<multiple_scored_cloud_payload>(multiple_scored_cloud_payload{
                .point_clouds = complete_payload,
                .color_map = color_map_func,
        });
    }

    auto identity_factor(const observed_point& point) -> double {
        return 1;
    }
}
