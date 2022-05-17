#pragma once

#include "types.h"
#include "scored_point.h"
#include "utils/projection.h"

typedef struct relative_point {
    Vector3d position;
    point_id_t id = 0;
    double distance = 0;
    double vertical_angle = 0;
    double horizontal_angle = 0;
} relative_point;

vector<relative_point> get_point_angles(const Image& image,
                                        double height_offset,
                                        const scored_point_map& points);

double find_interval_match(double search_value,
                           const vector<double>& mapped_values,
                           double first_range,
                           double range_epsilon);