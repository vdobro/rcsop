#include "simple_point.h"

namespace rcsop::common {

    SimplePoint::SimplePoint(point_id_t id, vec3 position) :
            _point_id(id), _position(std::move(position)) {}
            
    vec3 SimplePoint::position() const {
        return this->_position;
    }

    point_id_t SimplePoint::id() const {
        return this->_point_id;
    }

}