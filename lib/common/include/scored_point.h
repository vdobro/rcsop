#ifndef RCSOP_COMMON_SCORED_POINT_H
#define RCSOP_COMMON_SCORED_POINT_H

#include "utils/types.h"
#include "utils/points.h"
#include "simple_point.h"

namespace rcsop::common {
    using rcsop::common::SimplePoint;

    struct ScoreRange {
        double min;
        double max;
    };

    const double default_point_score = 0;

    class ScoredPoint : public IdPoint {
    private:
        SimplePoint _point;
        double _score{default_point_score};

    public:
        ScoredPoint() = default;

        ~ScoredPoint() override = default;

        ScoredPoint(vec3 position, point_id_t id, double score = default_point_score);

        ScoredPoint(const ScoredPoint& other) noexcept: ScoredPoint(other.position(), other.id(), other._score) {}

        ScoredPoint(ScoredPoint&& other) noexcept {
            *this = std::move(other);
        }

        ScoredPoint& operator=(const ScoredPoint& other) noexcept {
            if (this == &other)
                return *this;
            this->_point = other._point;
            this->_score = other._score;
            return *this;
        }

        ScoredPoint& operator=(ScoredPoint&& other) noexcept {
            if (this == &other)
                return *this;
            this->_point = other._point;
            this->_score = other._score;
            return *this;
        }

        [[nodiscard]] vec3 position() const override;

        [[nodiscard]] point_id_t id() const override;

        [[nodiscard]] double score_to_dB() const;

        [[nodiscard]] bool is_discarded() const;

        [[nodiscard]] static ScoreRange get_score_range(const vector<ScoredPoint>& points);
    };
}

#endif //RCSOP_COMMON_SCORED_POINT_H