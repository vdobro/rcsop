#ifndef RCSOP_COMMON_SCORED_POINT_H
#define RCSOP_COMMON_SCORED_POINT_H

#include "utils/types.h"
#include "utils/points.h"
#include "id_point.h"

namespace rcsop::common {
    using rcsop::common::IdPoint;

    struct ScoreRange {
        double min;
        double max;
    };

    class ScoredPoint {
    private:
        IdPoint _point;
        double _score = 0;

        double score() const {
            return _score;
        }
    public:
        ScoredPoint() = default;

        ScoredPoint(vec3 position, point_id_t id, double score);

        ScoredPoint(const ScoredPoint& other) : ScoredPoint(other.position(), other.id(), other.score()) {}

        ScoredPoint& operator=(const ScoredPoint& other) {
            if (this == &other)
                return *this;
            this->_point = other._point;
            return *this;
        }

        [[nodiscard]] vec3 position() const;

        [[nodiscard]] point_id_t id() const;

        [[nodiscard]] double score_to_dB() const;

        [[nodiscard]] bool is_discarded() const;

        static ScoreRange get_score_range(const vector<ScoredPoint>& points);

        virtual ~ScoredPoint() = default;
    };
}

#endif //RCSOP_COMMON_SCORED_POINT_H