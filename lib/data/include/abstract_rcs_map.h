#ifndef RCSOP_DATA_ABSTRACT_RCS_MAP_H
#define RCSOP_DATA_ABSTRACT_RCS_MAP_H

#include "utils/types.h"

#include "observed_point.h"
#include "observer.h"

class AbstractDataSet {
public:
    [[nodiscard]] virtual double map_to_nearest(const observed_point& point) const = 0;
};

class AbstractDataCollection {
public:
    [[nodiscard]] virtual shared_ptr<AbstractDataSet> get_for_exact_position(const Observer& observer) const = 0;
};

#endif //RCSOP_DATA_ABSTRACT_RCS_MAP_H