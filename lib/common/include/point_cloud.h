#ifndef SFM_COLORS_POINT_CLOUD_H
#define SFM_COLORS_POINT_CLOUD_H

#include <vector>
using std::vector;

#include "scored_point.h"

class PointCloud {
public:
    virtual std::vector<scored_point> points() = 0;
};

#endif //SFM_COLORS_POINT_CLOUD_H
