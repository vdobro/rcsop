#include "utils/rcs.h"

#include <cmath>

namespace rcsop::common::utils::rcs {

    auto raw_rcs_to_dB(rcs_value_t value) -> double {
        return 10.0 * log10(value);
    }
}