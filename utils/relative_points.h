#pragma once

#include "utils/types.h"
#include "utils/projection.h"

#include "scored_point.h"

typedef struct relative_point {
    Vector3d position;
    double distance_to_horizontal_plane = 0;
    point_id_t id = 0;
    double distance = 0;
    double vertical_angle = 0;
    double horizontal_angle = 0;
} relative_point;

vector<relative_point> get_point_angles(const Image& image,
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