#pragma once

#include "utils.h"

class scored_point {

private:
    ulong _point_id = -1;
    Vector3d _position = Vector3d::Zero();
    Vector3ub _color = Vector3ub::Zero();
    double _score = 0;

public:
    explicit scored_point() = default;
    explicit scored_point(const point_pair& base);

    void increment_score(double value);

    [[nodiscard]] Eigen::Vector2d flat_down() const;

    [[nodiscard]] Vector3d position() const;

    [[nodiscard]] Vector3ub color() const;

    [[nodiscard]] double score() const;

    [[nodiscard]] ulong id() const;
};
