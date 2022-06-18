#include "image_point.h"

ImagePoint::ImagePoint(Vector2d coordinates, double distance, double score)
        : _coordinates(std::move(coordinates)), _distance(distance), _score(score) {}

double ImagePoint::score() const {
    return _score;
}

double ImagePoint::distance() const {
    return _distance;
}

Vector2d ImagePoint::coordinates() const {
    return _coordinates;
}
