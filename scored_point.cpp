#include "scored_point.h"

scored_point::scored_point(Vector3d position, ulong id)
        : _point_id(id), _position(std::move(position)) {}

scored_point::scored_point(Vector3d position, ulong id, double score)
        : _point_id(id), _position(std::move(position)), _score(score) {}

scored_point::scored_point(const point_pair& base) {
    this->_point_id = base.first;
    this->_position = base.second.XYZ();
}

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
