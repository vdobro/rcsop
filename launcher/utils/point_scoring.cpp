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

    using rcsop::common::utils::gauss::rcs_gaussian_vertical;

    using rcsop::common::utils::logging::construct_log_prefix;

    using rcsop::common::utils::map_vec_shared;
    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::filter_vec_shared;

    using rcsop::common::ScoredPoint;
    using rcsop::common::SimplePoint;
    using rcsop::common::Observer;
    using rcsop::common::observed_factor_func;

    using rcsop::data::AbstractDataSet;
    using rcsop::data::PointCloudProvider;
    using rcsop::data::ObserverProvider;
    using rcsop::data::DataPointProjector;
    using rcsop::data::projection_options;
    using rcsop::data::ReconstructionType;

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

    struct filtered_scored_points {
        size_t total_count;
        shared_ptr<vector<ScoredPoint>> filtered_points;
    };

    static auto generate_base_points(const InputDataCollector& inputs,
                                     const task_options& task_options) -> shared_ptr<vector<SimplePoint>> {
        const auto point_provider = make_unique<PointCloudProvider>(inputs, task_options.camera);
        switch (task_options.point_generator) {
            case MODEL_SPARSE:
                return point_provider->get_base_points(ReconstructionType::SPARSE_CLOUD);
            case MODEL_DENSE:
                return point_provider->get_base_points(ReconstructionType::DENSE_CLOUD);
            case FULL_MODEL:
            case MODEL_WITH_PROJECTION:
                return point_provider->get_base_points(ReconstructionType::COMPLETE);
            case BOUNDING_BOX:
                return point_provider->generate_homogenous_cloud(task_options.point_density);
            case DATA_PROJECTION:
                return make_shared<vector<SimplePoint>>();
        }
        throw invalid_argument("task_options");
    }

    static inline auto score_observed_points(
            const vector<observed_point>& observed_points,
            const AbstractDataSet* data_for_observer,
            const projection_options& projection_options
    ) -> shared_ptr<vector<ScoredPoint>> {
        const auto vertical_angle_limit = projection_options.vertical_angle_limit;
        const auto& factor_func = projection_options.factor_func;
        auto scored_points = map_vec<observed_point, ScoredPoint, true>(
                observed_points,
                [&data_for_observer, &factor_func, &vertical_angle_limit](const observed_point& point) {
                    double value = data_for_observer->map_to_nearest(point);
                    if (abs(point.vertical_angle) > vertical_angle_limit || std::isnan(value)) {
                        return ScoredPoint(point.position, point.id, 0);
                    }

                    double factor = factor_func(point);
                    double final_value = factor * value;
                    return ScoredPoint(point.position, point.id, final_value);
                });
        auto result = make_shared<vector<ScoredPoint>>();
        for (const auto& point : scored_points) {
            if (!point.is_discarded()) {
                result->push_back(point);
            }
        }
        return result;
    }

    static auto filter_and_score_points(
            const AbstractDataSet* data_for_observer,
            const vector<SimplePoint>& base_points,
            const Observer& observer,
            const projection_options& projection_params
    ) -> filtered_scored_points {
        const auto observed_points = observer.observe_points(base_points);
        const auto scored_points = score_observed_points(*observed_points, data_for_observer, projection_params);
        auto filtered_points = filter_points(*scored_points, projection_params.db_filter);
        return {
            .total_count = observed_points->size(),
            .filtered_points = filtered_points,
        };
    }

    static auto project_data_to_points(
            const AbstractDataSet* data_for_observer,
            const Observer& observer,
            const DataPointProjector& projector,
            const data::projection_options& projection_params) -> filtered_scored_points {
        auto projected_points = projector.project_data(
                data_for_observer, observer, projection_params);
        auto total_count = projected_points->size();
        auto filtered_points = filter_points(*projected_points, projection_params.db_filter);
        return {
                .total_count = total_count,
                .filtered_points = filtered_points
        };
    }

    auto score_points(
            const InputDataCollector& inputs,
            const vector<data_with_observer_options>& labeled_data,
            const task_options& task_options,
            const global_colormap_func& color_map_func) -> shared_ptr<multiple_scored_cloud_payload const> {
        auto dB_range = task_options.db_range;
        auto observer_provider = make_shared<ObserverProvider>(inputs, task_options.camera, true);
        auto projector = make_shared<DataPointProjector>();

        auto observers = observer_provider->observers_with_positions();
        auto observer_count = observers.size();
        auto base_points = generate_base_points(inputs, task_options);

        auto total_time = start_time();
        auto range_filter = get_range_filter(dB_range);

        auto vertical_distribution = rcs_gaussian_vertical(task_options.vertical_options.angle_spread,
                                                           task_options.vertical_options.normal_variance);
        auto projection_params = data::projection_options {
            .db_filter = range_filter,
            .factor_func = vertical_distribution,
            .vertical_angle_limit = task_options.vertical_options.angle_spread,
            .steps_per_angle = task_options.point_density,
        };

        size_t total_count{0};
        size_t filtered_count{0};
        auto complete_payload = map_vec<Observer, ScoredCloud, false>(
                observers,
                [&labeled_data, &task_options,
                 &base_points, &projector, &observer_count, &total_count, &filtered_count, &projection_params]
                        (const size_t index, const Observer& observer) {
                    auto time = start_time();
                    auto relevant_points = make_shared<vector<ScoredPoint>> ();
                    for (const auto& [observer_options, data_collection]: labeled_data) {
                        auto data_for_observer = data_collection->get_for_exact_position(observer);
                        auto observer_with_translation = observer.clone_with_data(observer_options);

                        // 1. observed base points
                        if (!base_points->empty()) {
                            auto [total_observed_count, observed_points] = filter_and_score_points(
                                    data_for_observer, *base_points, observer_with_translation, projection_params);
                            total_count += total_observed_count;
                            filtered_count += observed_points->size();

                            relevant_points->insert(relevant_points->end(),
                                                    observed_points->cbegin(), observed_points->cend());
                        }

                        // 2. projected points
                        if ((task_options.point_generator & PointGenerator::DATA_PROJECTION) != 0) {
                            auto [total_projected_count, projected_points] = project_data_to_points(
                                    data_for_observer, observer_with_translation, *projector, projection_params);
                            total_count += total_projected_count;
                            filtered_count += projected_points->size();

                            relevant_points->insert(relevant_points->end(),
                                                    projected_points->cbegin(), projected_points->cend());
                        }
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
}
