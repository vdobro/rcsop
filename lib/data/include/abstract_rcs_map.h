#ifndef RCSOP_DATA_ABSTRACT_RCS_MAP_H
#define RCSOP_DATA_ABSTRACT_RCS_MAP_H

#include "utils/types.h"

#include "observer_position.h"
#include "observed_point.h"

class AbstractDataSet {
public:
    [[nodiscard]] virtual double map_to_nearest(const observed_point& point) const = 0;
};

class AbstractDataCollection {
public:
    [[nodiscard]] virtual shared_ptr<AbstractDataSet> at_position(const ObserverPosition& position) const = 0;
};

#endif //RCSOP_DATA_ABSTRACT_RCS_MAP_H