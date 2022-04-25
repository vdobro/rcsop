#pragma once

#include <util/types.h>
#include "utils.h"

class space_point {

private:
    ulong _point_id;
    Eigen::Vector3d _position;
    Eigen::Vector3ub _color;

public:
    explicit space_point(const point_pair& base);

    [[nodiscard]] Eigen::Vector2d flat_down() const;

    Eigen::Vector3ub& color();

    [[nodiscard]] ulong id() const;
};
