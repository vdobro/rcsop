#include "scored_point.h"

#include "utils.h"

scored_point::scored_point(const point_pair& base) {
    this->_point_id = base.first;
    this->_position = base.second.XYZ();
    this->_color = base.second.Color();
}

Eigen::Vector2d scored_point::flat_down() const {
    return flat_down_from_above(this->_position);
}

point_id_t scored_point::id() const { return _point_id; }

Vector3d scored_point::position() const {
    return _position;
}

Vector3ub scored_point::color() const {
    return _color;
}

double scored_point::score() const {
    return _score;
}

void scored_point::increment_score(double value) {
    this->_score += value;
}

