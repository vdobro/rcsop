#pragma once

#include <cmath>
#include <stdexcept>
#include <memory>
#include <map>

#include "matio.h"
#include "relative_points.h"
#include "observer_position.h"

using std::map;
using std::string;
using std::runtime_error;
using std::invalid_argument;

class az_data {
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
    explicit az_data(const string& filename,
                     const ObserverPosition& position);

    [[nodiscard]] map<double, vector<double>> get_rcs() const;

    [[nodiscard]] ObserverPosition get_position() const;

    [[nodiscard]] double find_nearest(double range_distance, double angle) const;
};
