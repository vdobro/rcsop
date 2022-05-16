#pragma once

#include <stdexcept>
#include <memory>
#include <map>

#include "utils.h"
#include "matio.h"

using std::map;
using std::runtime_error;
using std::invalid_argument;

class az_data {
private:
    vector<double> _ranges;
    vector<double> _angles;
    map<double, vector<double>> _angle_to_rcs_values;

    map<double, vector<double>> reconstruct_azimuth_table(const vector<double>& raw_values);

public:
    explicit az_data(const std::string& filename);

    [[nodiscard]] map<double, vector<double>> get_rcs() const;
};
