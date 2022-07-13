#include "image_point.h"

namespace rcsop::common {
    ImagePoint::ImagePoint(vec2 coordinates,
                           double distance,
                           double score)
            : _coordinates(std::move(coordinates)),
              _distance(distance),
              _score(score) {}

    double ImagePoint::score() const {
        return _score;
    }

    double ImagePoint::distance() const {
        return _distance;
    }

    vec2 ImagePoint::coordinates() const {
        return _coordinates;
    }
}
