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

    auto ObserverPosition::distance_to(const ObserverPosition& other,
                                       const double world_distance_to_origin) const -> double {
        auto abs_angle_diff = abs(azimuth - other.azimuth);
        auto angle_diff = static_cast<double>(min(abs_angle_diff, 360 - abs_angle_diff));
        auto half_angle_diff = angle_diff / 2.;
        auto angle_sinus = sin(half_angle_diff * M_PI / 180.);
        auto distance_in_circle = 2 * world_distance_to_origin * angle_sinus;

        auto height_difference = static_cast<double>(abs(height - other.height));
        return sqrt(distance_in_circle * distance_in_circle + height_difference * height_difference);
    }

    bool ObserverPosition::operator<(const ObserverPosition& other) const {
        return (height < other.height) || (height == other.height && azimuth < other.azimuth);
    }
}