#ifndef RCSOP_DATA_ABSTRACT_RCS_MAP_H
#define RCSOP_DATA_ABSTRACT_RCS_MAP_H

#include "utils/types.h"
#include "utils/rcs.h"

#include "observed_point.h"
#include "observer.h"
#include "observer_position.h"

namespace rcsop::data {
    using rcsop::common::observed_point;
    using rcsop::common::Observer;
    using rcsop::common::height_t;
    using rcsop::common::ObserverPosition;

    using rcsop::common::utils::rcs::rcs_value_t;
    using rcsop::common::utils::rcs::rcs_distance_t;
    using rcsop::common::utils::rcs::rcs_angle_t;

    class AbstractDataSet {
    public:
        [[nodiscard]] virtual auto map_to_nearest(const observed_point& point) const -> rcs_value_t = 0;

        [[nodiscard]] virtual auto map_exact(rcs_distance_t distance, rcs_angle_t angle) const -> rcs_value_t = 0;

        [[nodiscard]] virtual auto distances() const -> vector<rcs_distance_t> = 0;

        [[nodiscard]] virtual auto distance_step() const -> rcs_distance_t = 0;

        [[nodiscard]] virtual auto angles() const -> vector<rcs_angle_t> = 0;

        virtual ~AbstractDataSet() = default;
    };

    class AbstractDataCollection {
    public:
        [[nodiscard]] virtual auto get_for_exact_position(const Observer& observer) const -> const AbstractDataSet* = 0;

        [[nodiscard]] virtual auto heights() const -> vector<height_t> = 0;

        [[nodiscard]] virtual auto available_positions() const -> vector<ObserverPosition> = 0;

        virtual ~AbstractDataCollection() = default;
    };
}

#endif //RCSOP_DATA_ABSTRACT_RCS_MAP_H