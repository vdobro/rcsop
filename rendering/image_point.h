#pragma once

#include "utils/types.h"

class image_point {
private:
    Vector2d _coordinates = Vector2d::Zero();
    double _distance = 0;
    double _score = 0;
public:
    explicit image_point() = default;

    explicit image_point(Vector2d coordinates, double distance, double score);

    [[nodiscard]] double score() const;

    [[nodiscard]] double distance() const;

    [[nodiscard]] Vector2d coords() const;
};