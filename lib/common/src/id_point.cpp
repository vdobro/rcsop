#include "id_point.h"

namespace rcsop::common {

    IdPoint::IdPoint(point_id_t id, vec3 position) :
            _point_id(id), _position(std::move(position)) {}
            
    vec3 IdPoint::position() const {
        return this->_position;
    }

    point_id_t IdPoint::id() const {
        return this->_point_id;
    }

}