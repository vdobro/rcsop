#ifndef RCSOP_COMMON_IMAGE_POINT_H
#define RCSOP_COMMON_IMAGE_POINT_H

#include "utils/types.h"
#include "utils/points.h"

namespace rcsop::common {
    using rcsop::common::utils::points::vec2;

    class ImagePoint {
    private:
        vec2 _coordinates = vec2::Zero();
        double _distance = 0;
        double _score = 0;
    public:
        explicit ImagePoint() = default;

        explicit ImagePoint(vec2 coordinates, double distance, double score);

        [[nodiscard]] double score() const;

        [[nodiscard]] double distance() const;

        [[nodiscard]] vec2 coordinates() const;
    };
}
#endif //RCSOP_COMMON_IMAGE_POINT_H
