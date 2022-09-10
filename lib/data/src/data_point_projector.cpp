#include "data_point_projector.h"

namespace rcsop::data {
    using common::utils::points::point_id_t;
    using common::utils::rcs::raw_rcs_to_dB;
    using uniform_distribution = std::uniform_real_distribution<double>;

    const double STEP_DISTANCE = 3;
    const double STEP_ANGLE = 0.3;
    const double VERTICAL_ANGLE_LIMIT = 5;

    DataPointProjector::DataPointProjector() = default;

    static auto combine_ranges(const vector<double>& angles,
                               const vector<double>& distances,
                               const double angle_step,
                               const double distance_environment,
                               const double secondary_angle_limit) -> vector<observed_point> {
        std::random_device rd;
        std::mt19937 e2(rd());
        auto half_step{angle_step / 2.};
        uniform_distribution angle_noise(-half_step, half_step);
        uniform_distribution distance_noise(-distance_environment, distance_environment);

        vector<observed_point> points;
        for (auto primary_angle: angles) {
            for (auto ranged_distance: distances) {
                double secondary_angle{-secondary_angle_limit};
                while (secondary_angle <= secondary_angle_limit) {
                    auto horizontal_angle = primary_angle + angle_noise(e2);
                    auto vertical_angle = secondary_angle + angle_noise(e2);
                    auto distance = ranged_distance + distance_noise(e2);
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
                                          const function<bool(double)>& db_filter,
                                          const observed_factor_func& factor_func,
                                          const Observer& observer) const
    -> shared_ptr<vector<ScoredPoint>> {
        const auto angles = data->angles();
        const auto distances = data->distances();
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

                auto distance_range = get_range(distances, distance_idx, STEP_DISTANCE);
                auto angle_range = get_range(angles, angle_idx, STEP_ANGLE);
                auto points_in_ranges = combine_ranges(angle_range, distance_range,
                                                       STEP_ANGLE, STEP_DISTANCE / 2 - 0.001, VERTICAL_ANGLE_LIMIT);
                for (auto& point: points_in_ranges) {
                    point.id = id++;
                    point.position = observer.project_position(point);

                    auto score = data_point * factor_func(point);
                    auto scored_point = ScoredPoint(point.position, point.id, score);
                    points->push_back(scored_point);
                }
            }
        }
        return points;
    }
}
