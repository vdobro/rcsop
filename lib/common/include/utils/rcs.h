#ifndef RCSOP_COMMON_RCS_H
#define RCSOP_COMMON_RCS_H

namespace rcsop::common::utils::rcs {

    using rcs_value_t = double;
    using rcs_angle_t = double;
    using rcs_distance_t = long;

    auto raw_rcs_to_dB(rcs_value_t value) -> double;
}

#endif //RCSOP_COMMON_RCS_H
