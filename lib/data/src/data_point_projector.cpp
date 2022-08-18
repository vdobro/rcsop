#include "data_point_projector.h"

namespace rcsop::data {
    using common::utils::points::point_id_t;

    const double STEP_DISTANCE = 3;
    const double STEP_ANGLE = 0.3;
    const double VERTICAL_ANGLE_LIMIT = 5;

    DataPointProjector::DataPointProjector() = default;

    static auto combine_ranges(const vector<double>& angles,
                               const vector<double>& distances,
                               const double angle_step,
                               const double secondary_angle_limit) -> vector<observed_point> {
        std::random_device rd;
        std::mt19937 e2(rd());
        auto half_step{angle_step / 2.};
        std::uniform_real_distribution<double> dist(-half_step, half_step);

        vector<observed_point> points;
        for (auto angle: angles) {
            for (auto distance: distances) {
                double secondary_angle{-secondary_angle_limit};
                while (secondary_angle <= secondary_angle_limit) {
                    observed_point point{
                            .position = common::utils::points::vec3::Zero(),
                            .id = 0,
                            .distance_in_world = distance,
                            .vertical_angle = secondary_angle + dist(e2),
                            .horizontal_angle = angle + dist(e2),
                    };
                    points.push_back(point);

                    secondary_angle += angle_step;
                }
            }
        }
        return points;
    }

    auto DataPointProjector::project_data(const AbstractDataSet* data,
                                          const function<bool(const rcs_value_t)>& filter,
                                          const Observer& observer) const
    -> shared_ptr<vector<ScoredPoint>> const {
        const auto angles = data->angles();
        const auto distances = data->distances();

        auto points = make_shared<vector<ScoredPoint>>();
        point_id_t id{0};
        for (size_t distance_idx{0}; distance_idx < distances.size(); distance_idx++) {
            const auto distance = distances.at(distance_idx);

            for (size_t angle_idx{0}; angle_idx < angles.size(); angle_idx++) {
                const auto angle = angles.at(angle_idx);
                const auto data_point = data->map_exact(distance, angle);
                if (!filter(data_point)) {
                    continue;
                }

                const auto distance_range = get_range(distances, distance_idx, STEP_DISTANCE);
                const auto angle_range = get_range(angles, angle_idx, STEP_ANGLE);
                const auto points_in_ranges = combine_ranges(angle_range, distance_range,
                                                             STEP_ANGLE, VERTICAL_ANGLE_LIMIT);
                for (auto point: points_in_ranges) {
                    point.id = id++;
                    point.position = observer.project_position(point);

                    auto scored_point = ScoredPoint(point.position, point.id);
                    auto observation = observer.observe_point(scored_point);
                    assert(observation.horizontal_angle == point.horizontal_angle);
                    assert(observation.vertical_angle == point.vertical_angle);
                    assert(observation.distance_in_world == point.distance_in_world);
                    points->push_back(scored_point);
                }
            }
        }
        return points;
    }
}
