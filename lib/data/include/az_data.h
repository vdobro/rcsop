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

using az_value_map_t = map<double, vector<double>>;
class AzimuthRcsDataSet : public AbstractDataSet {
private:
    ObserverPosition _position{};
    vector<long> _ranges;
    long _range_step{};

    vector<double> _angles;
    double _angle_step{};

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

    [[nodiscard]] double map_to_nearest(const observed_point& point) const override;

    void use_filtered_peaks();
};

#endif //RCSOP_DATA_AZ_DATA_H
