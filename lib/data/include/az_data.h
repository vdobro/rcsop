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

class AzimuthRcsDataSet : public AbstractDataSet {
private:
    ObserverPosition _position{};
    vector<long> _ranges;
    long _range_step{};

    vector<double> _angles;
    double _angle_step{};

    map<double, vector<double>> _angle_to_rcs_values;

    map<double, vector<double>> reconstruct_value_table(const vector<double>& raw_values);

    void determine_step_sizes();

public:
    explicit AzimuthRcsDataSet(const path& filename,
                               const ObserverPosition& position);

    [[nodiscard]] double map_to_nearest(const observed_point& point) const override;
};

#endif //RCSOP_DATA_AZ_DATA_H
