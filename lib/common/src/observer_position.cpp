#include "observer_position.h"

#include <iomanip>
#include <sstream>

namespace rcsop::common {
    using std::stringstream;

    auto ObserverPosition::str() const -> string {
        stringstream ss;
        ss << height << "cm_";
        ss << std::setw(3) << std::setfill('0') << azimuth;
        ss << "°";
        return ss.str();
    }
}