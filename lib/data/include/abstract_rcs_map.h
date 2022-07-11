#ifndef RCSOP_DATA_ABSTRACT_RCS_MAP_H
#define RCSOP_DATA_ABSTRACT_RCS_MAP_H

#include "utils/types.h"

#include "observed_point.h"
#include "observer.h"

namespace rcsop::data {
    using rcsop::common::observed_point;
    using rcsop::common::Observer;

    using rcs_value_t = double;

    class AbstractDataSet {
    public:
        [[nodiscard]] virtual rcs_value_t map_to_nearest(const observed_point& point) const = 0;
    };

    class AbstractDataCollection {
    public:
        [[nodiscard]] virtual const AbstractDataSet* get_for_exact_position(const Observer& observer) const = 0;
    };
}

#endif //RCSOP_DATA_ABSTRACT_RCS_MAP_H