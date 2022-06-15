#ifndef RCSOP_DATA_RELATIVE_POINTS_H
#define RCSOP_DATA_RELATIVE_POINTS_H

#include "utils/types.h"
#include "utils/vector.h"

#include "scored_point.h"
#include "camera.h"
#include "observed_point.h"

vector<observed_point> get_point_angles(const camera& camera,
                                        double height_offset,
                                        const scored_point_map& points);

template<typename InputValueType, typename SearchValue>
size_t find_interval_match_index(InputValueType search_value,
                                 SearchValue first_range,
                                 SearchValue range_epsilon) {
    return lround((static_cast<SearchValue>(search_value) - first_range) / (2 * range_epsilon));
}

template<typename InputValueType, typename SearchValue, typename MappedValue>
MappedValue find_interval_match(InputValueType search_value,
                                const vector<MappedValue>& mapped_values,
                                SearchValue first_range,
                                SearchValue range_epsilon) {
    size_t index = find_interval_match_index(search_value, first_range, range_epsilon);
    return mapped_values.at(index);
}

#endif //RCSOP_DATA_RELATIVE_POINTS_H
