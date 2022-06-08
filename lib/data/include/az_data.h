#pragma once

#include <cmath>
#include <stdexcept>
#include <memory>
#include <map>

#include "matio.h"
#include "relative_points.h"

using std::map;
using std::string;
using std::runtime_error;
using std::invalid_argument;

typedef struct data_eval_position {
    long height;
    long azimuth;
} data_eval_position;

class az_data {
private:
    data_eval_position _position{};
    vector<long> _ranges;
    long _range_step{};

    vector<double> _angles;
    double _angle_step{};

    map<double, vector<double>> _angle_to_rcs_values;

    map<double, vector<double>> reconstruct_value_table(const vector<double>& raw_values);

    void determine_step_sizes();
public:
    explicit az_data(const string& filename,
                     const data_eval_position& position);

    [[nodiscard]] map<double, vector<double>> get_rcs() const;

    [[nodiscard]] data_eval_position get_position() const;

    [[nodiscard]] double find_nearest(double range_distance, double angle) const;
};
