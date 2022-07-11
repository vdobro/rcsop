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

    using az_angle_t = double;
    using az_range_t = long;

    using az_value_map_t = map<az_angle_t, vector<rcs_value_t>>;

    class AzimuthRcsDataSet : public AbstractDataSet {
    private:
        ObserverPosition _position{};
        vector<az_range_t> _ranges;
        az_range_t _range_step{};

        vector<az_angle_t> _angles;
        az_angle_t _angle_step{};

        az_value_map_t _raw_values;
        az_value_map_t _filtered_values;
        bool use_filtered = false;

        az_value_map_t reconstruct_value_table(const vector<double>& raw_values);

        void determine_step_sizes();

        void filter_peaks();

    public:
        AzimuthRcsDataSet(const path& filename,
                          const ObserverPosition& position);

        ~AzimuthRcsDataSet() = default;

        [[nodiscard]] rcs_value_t map_to_nearest(const observed_point& point) const override;

        void use_filtered_peaks();
    };
}

#endif //RCSOP_DATA_AZ_DATA_H
