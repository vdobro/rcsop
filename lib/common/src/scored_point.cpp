#include "scored_point.h"
#include "utils/vector.h"

#include <utility>

scored_point::scored_point(Vector3d position, ulong id)
        : _point_id(id), _position(std::move(position)) {}

scored_point::scored_point(Vector3d position, ulong id, double score)
        : _point_id(id), _position(std::move(position)), _score(score) {}

scored_point::scored_point(const point_pair& base)
        : _point_id(base.first), _position(base.second) {}

point_id_t scored_point::id() const { return _point_id; }

Vector3d scored_point::position() const {
    return _position;
}

double scored_point::score() const {
    return _score;
}

double scored_point::score_to_dB() const {
    return 10.0 * log10(this->score());
}

void scored_point::increment_score(double value) {
    this->_score += value;
}

ScoreRange scored_point::get_score_range(const vector<scored_point>& points) {
    if (points.empty()) {
        throw std::invalid_argument("Point array may not be empty to find a min/max value.");
    }
    auto scores = map_vec<scored_point, double>(points, &scored_point::score_to_dB);

    auto min_score = *std::min_element(scores.begin(), scores.end());
    auto max_score = *std::max_element(scores.begin(), scores.end());

    return ScoreRange{
            .min = min_score,
            .max = max_score,
    };
}
