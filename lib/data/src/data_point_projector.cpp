#include "data_point_projector.h"

#include "utils/random.h"

namespace rcsop::data {
    using rcsop::common::utils::points::point_id_t;
    using rcsop::common::utils::rcs::raw_rcs_to_dB;
    using rcsop::common::utils::get_uniform_distribution;

    static auto combine_ranges(const vector<double>& angles,
                               const vector<double>& distances,
                               const double angle_step,
                               const double distance_epsilon,
                               const double secondary_angle_limit) -> vector<observed_point> {

        auto half_step{angle_step / 2.};
        auto angle_noise = get_uniform_distribution(half_step, true);
        auto distance_noise = get_uniform_distribution(distance_epsilon, true);

        vector<observed_point> points;
        for (auto primary_angle: angles) {
            for (auto ranged_distance: distances) {
                double secondary_angle{-secondary_angle_limit};
                while (secondary_angle <= secondary_angle_limit) {
                    auto horizontal_angle = primary_angle + angle_noise();
                    auto vertical_angle = secondary_angle + angle_noise();
                    auto distance = ranged_distance + distance_noise();
                    secondary_angle += angle_step;

                    if (horizontal_angle < -90 || horizontal_angle > 90) {
                        continue;
                    }
                    observed_point point{
                            .position = common::utils::points::vec3::Zero(),
                            .id = 0,
                            .distance_in_world = distance,
                            .vertical_angle = vertical_angle,
                            .horizontal_angle = horizontal_angle,
                    };
                    points.push_back(point);
                }
            }
        }
        return points;
    }

    auto DataPointProjector::project_data(const AbstractDataSet* data,
                                          const Observer& observer,
                                          const projection_options& projection_params) const
    -> shared_ptr<vector<ScoredPoint>> {
        const auto angles = data->angles();
        const auto angle_step = 1. / static_cast<double>(projection_params.steps_per_angle);
        const auto distances = data->distances();
        const auto distance_step = static_cast<double>(data->distance_step());
        const auto& db_filter = projection_params.db_filter;

        auto data_filter = [&db_filter](rcs_value_t rcs_value) -> bool {
            const auto db_value = raw_rcs_to_dB(rcs_value);
            return db_filter(db_value);
        };

        auto points = make_shared<vector<ScoredPoint>>();
        point_id_t id{0};
        for (size_t angle_idx{0}; angle_idx < angles.size(); angle_idx++) {
            const auto angle = angles.at(angle_idx);
            for (size_t distance_idx{0}; distance_idx < distances.size(); distance_idx++) {
                auto distance = distances.at(distance_idx);

                auto data_point = data->map_exact(distance, angle);
                if (!data_filter(data_point)) {
                    continue;
                }

                auto distance_range = get_range(distances, distance_idx, distance_step);
                auto angle_range = get_range(angles, angle_idx, angle_step);
                auto points_in_ranges = combine_ranges(angle_range,
                                                       distance_range,
                                                       angle_step,
                                                       distance_step / 2 - STANDARD_ERROR,
                                                       projection_params.vertical_angle_limit);
                for (auto& point: points_in_ranges) {
                    point.id = id++;
                    point.position = observer.project_position(point);

                    auto score = data_point * projection_params.factor_func(point);
                    auto scored_point = ScoredPoint(point.position, point.id, score);
                    points->push_back(scored_point);
                }
            }
        }
        return points;
    }
}
