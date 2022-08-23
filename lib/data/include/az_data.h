#ifndef RCSOP_DATA_AZ_DATA_H
#define RCSOP_DATA_AZ_DATA_H

#include <cmath>
#include <stdexcept>
#include <memory>
#include <map>

#include "matio.h"

#include "utils/types.h"

#include "observer_position.h"
#include "observed_point.h"
#include "abstract_rcs_map.h"

namespace rcsop::data {
    using rcsop::common::ObserverPosition;

    using az_value_map_t = map<rcs_angle_t, vector<rcs_value_t>>;

    class AzimuthRcsDataSet : public AbstractDataSet {
    private:
        vector<rcs_distance_t> _ranges;
        vector<rcs_angle_t> _angles;

        az_value_map_t _raw_values;
        az_value_map_t _filtered_values;
        bool use_filtered = false;

        [[nodiscard]] az_value_map_t reconstruct_value_table(const vector<double>& raw_values);

        void filter_peaks();

        [[nodiscard]] rcs_value_t resolve_value(rcs_angle_t angle, size_t range_index) const;
    public:
        AzimuthRcsDataSet(const path& filename,
                          const ObserverPosition& position);

        [[nodiscard]] rcs_value_t map_to_nearest(const observed_point& point) const override;

        [[nodiscard]] rcs_value_t map_exact(rcs_distance_t distance, rcs_angle_t angle) const override;

        [[nodiscard]] vector<rcs_distance_t> distances() const override;

        [[nodiscard]] vector<rcs_angle_t> angles() const override;

        void use_filtered_peaks();
    };
}

#endif //RCSOP_DATA_AZ_DATA_H
