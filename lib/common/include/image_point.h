#ifndef RCSOP_COMMON_IMAGE_POINT_H
#define RCSOP_COMMON_IMAGE_POINT_H

#include "utils/types.h"
#include "utils/points.h"

class ImagePoint {
private:
    Vector2d _coordinates = Vector2d::Zero();
    double _distance = 0;
    double _score = 0;
public:
    explicit ImagePoint() = default;

    explicit ImagePoint(Vector2d coordinates, double distance, double score);

    [[nodiscard]] double score() const;

    [[nodiscard]] double distance() const;

    [[nodiscard]] Vector2d coordinates() const;
};

#endif //RCSOP_COMMON_IMAGE_POINT_H
