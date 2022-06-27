#ifndef RCSOP_COMMON_OBSERVED_POINT_H
#define RCSOP_COMMON_OBSERVED_POINT_H

#include "utils/types.h"
#include "utils/points.h"

struct observed_point {
    Vector3d position = Vector3d::Zero();
    point_id_t id = 0;
    double distance_in_world = 0;
    double vertical_angle = 0;
    double horizontal_angle = 0;
};

#endif //RCSOP_COMMON_OBSERVED_POINT_H
