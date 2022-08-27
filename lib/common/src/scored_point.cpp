#include "scored_point.h"

#include <utility>

#include "utils/mapping.h"
#include "utils/rcs.h"

namespace rcsop::common {
    using rcsop::common::utils::rcs::raw_rcs_to_dB;
    using rcsop::common::utils::map_vec;

    ScoredPoint::ScoredPoint(vec3 position, point_id_t id, double score) :
            _point(IdPoint(id, std::move(position))), _score(score) {}

    point_id_t ScoredPoint::id() const { return _point.id(); }

    vec3 ScoredPoint::position() const {
        return _point.position();
    }

    double ScoredPoint::score_to_dB() const {
        return raw_rcs_to_dB(_score);
    }

    ScoreRange ScoredPoint::get_score_range(const vector<ScoredPoint>& points) {
        if (points.empty()) {
            throw std::invalid_argument("Point array may not be empty to find a min/max value.");
        }
        auto scores = map_vec<ScoredPoint, double>(points, &ScoredPoint::score_to_dB);

        auto min_score = *std::min_element(scores.cbegin(), scores.cend());
        auto max_score = *std::max_element(scores.cbegin(), scores.cend());

        return ScoreRange{
                .min = min_score,
                .max = max_score,
        };
    }

    bool ScoredPoint::is_discarded() const {
        return this->_score == default_point_score || std::isnan(this->_score);
    }
}
