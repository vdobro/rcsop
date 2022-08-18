#ifndef RCSOP_DATA_DATA_POINT_PROJECTOR_H
#define RCSOP_DATA_DATA_POINT_PROJECTOR_H

#include "utils/types.h"
#include "abstract_rcs_map.h"

namespace rcsop::data {
    using rcsop::common::Observer;
    using rcsop::common::camera_options;
    using rcsop::common::ScoredPoint;
    using rcsop::common::utils::points::vec3;

    class DataPointProjector {
    private:

        template<typename ValueType>
        [[nodiscard]] auto get_range(const vector<ValueType>& source_values,
                                     const size_t index,
                                     double step_size) const -> vector<double> {
            assert(source_values.size() > 2);

            vector<double> result;
            const double first = source_values.at(0);
            const double last = source_values.at(source_values.size() - 1);

            const double distance_after_first = abs(source_values.at(1) - first);
            const double distance_before_last = abs(source_values.at(source_values.size() - 2) - last);

            double lower_bound = first - (distance_after_first / 2);
            double upper_bound = last + (distance_before_last / 2);

            const auto is_first = index == 0;
            const auto is_last = index == source_values.size() - 1;

            const auto current_value = source_values.at(index);

            if (!is_first) {
                const auto previous = source_values.at(index - 1);
                const auto distance_to_previous = abs(current_value - previous);
                lower_bound = current_value - distance_to_previous / 2;
            }
            if (!is_last) {
                const auto next = source_values.at(index + 1);
                const auto distance_to_next = abs(current_value - next);
                upper_bound = current_value + distance_to_next / 2;
            }

            auto value{lower_bound};
            while (value <= upper_bound) {
                result.push_back(value);
                value += step_size;
            }
            return result;
        }

    public:
        explicit DataPointProjector();

        auto project_data(const AbstractDataSet* data,
                          const function<bool(const rcs_value_t)>& filter,
                          const Observer& observer) const -> shared_ptr<vector<ScoredPoint>> const;

    };

} // data

#endif //RCSOP_DATA_DATA_POINT_PROJECTOR_H
