#pragma once

#include <stdexcept>
#include <memory>
#include <map>

#include "utils.h"
#include "matio.h"

using std::map;
using std::string;
using std::runtime_error;
using std::invalid_argument;

typedef struct {
    long height;
    long azimuth;
} data_eval_position;

class az_data {
private:
    data_eval_position _position;
    vector<double> _ranges;
    vector<double> _angles;
    map<double, vector<double>> _angle_to_rcs_values;

    map<double, vector<double>> reconstruct_value_table(const vector<double>& raw_values);

public:
    explicit az_data(const string& filename,
                     const data_eval_position& position);

    [[nodiscard]] map<double, vector<double>> get_rcs() const;

    [[nodiscard]] data_eval_position get_position() const;
};
