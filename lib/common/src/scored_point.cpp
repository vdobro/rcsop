#include "scored_point.h"
#include "utils/mapping.h"

#include <utility>

namespace rcsop::common {
    using rcsop::common::utils::map_vec;

    ScoredPoint::ScoredPoint(Vector3d position, point_id_t id, double score) :
            _point_id(id),
            _position(std::move(position)),
            _score(score) {}

    point_id_t ScoredPoint::id() const { return _point_id; }

    Vector3d ScoredPoint::position() const {
        return _position;
    }

    double ScoredPoint::score() const {
        return _score;
    }

    double ScoredPoint::score_to_dB() const {
        return 10.0 * log10(this->score());
    }

    ScoreRange ScoredPoint::get_score_range(const vector<ScoredPoint>& points) {
        if (points.empty()) {
            throw std::invalid_argument("Point array may not be empty to find a min/max value.");
        }
        auto scores = map_vec<ScoredPoint, double>(points, &ScoredPoint::score_to_dB);

        auto min_score = *std::min_element(scores.begin(), scores.end());
        auto max_score = *std::max_element(scores.begin(), scores.end());

        return ScoreRange{
                .min = min_score,
                .max = max_score,
        };
    }
}
