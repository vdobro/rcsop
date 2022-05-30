#include "image_point.h"

image_point::image_point(Vector2d coordinates, double distance, double score)
        : _coordinates(std::move(coordinates)), _distance(distance), _score(score) {}

double image_point::score() const {
    return _score;
}

double image_point::distance() const {
    return _distance;
}

Vector2d image_point::coords() const {
    return _coordinates;
}
