#include "space_point.h"

space_point::space_point(const point_pair& base) {
    this->_point_id = base.first;
    this->_position = base.second.XYZ();
    this->_color = base.second.Color();
}

Eigen::Vector3ub& space_point::color() {
    return _color;
}

Eigen::Vector2d space_point::flat_down() const {
    return flat_down_from_above(this->_position);
}

ulong space_point::id() const { return _point_id; }
