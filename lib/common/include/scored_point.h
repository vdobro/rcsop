#ifndef RCSOP_COMMON_SCORED_POINT_H
#define RCSOP_COMMON_SCORED_POINT_H

#include "utils/types.h"
#include "utils/points.h"

namespace rcsop::common {
    using rcsop::common::utils::points::point_id_t;
    using rcsop::common::utils::points::vec3;

    struct ScoreRange {
        double min;
        double max;
    };

    class ScoredPoint {

    private:
        point_id_t _point_id = -1;
        vec3 _position = vec3::Zero();
        double _score = 0;

    public:
        ScoredPoint() = default;

        ScoredPoint(vec3 position, point_id_t id, double score = 0);

        ScoredPoint(const ScoredPoint& other) : ScoredPoint(other.position(), other.id(), other.score()) {}

        ScoredPoint& operator=(const ScoredPoint& other) {
            if (this == &other)
                return *this;
            this->_point_id = other.id();
            this->_position = other.position();
            this->_score = other.score();
            return *this;
        }

        [[nodiscard]] vec3 position() const;

        [[nodiscard]] double score() const;

        [[nodiscard]] point_id_t id() const;

        [[nodiscard]] double score_to_dB() const;

        static ScoreRange get_score_range(const vector<ScoredPoint>& points);

        virtual ~ScoredPoint() = default;
    };
}

#endif //RCSOP_COMMON_SCORED_POINT_H