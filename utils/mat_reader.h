#pragma once

#include "matio.h"
#include "utils/utils.h"

using std::runtime_error;
using std::invalid_argument;

class rcs_data {
private:
    vector<double> _rcs;
    vector<double> _rcs_dbs;
    vector<long> _angles;
    vector<long> _ranges;
    map<long, vector<double>> _azimuth;
    map<long, vector<double>> _azimuth_db;

    rcs_data() = default;

    map<long, vector<double>> reconstruct_azimuth_table(const vector<double>& raw_values);

    map<long, vector<double>> get_azimuth(unsigned int index, matvar_t* table);

    map<long, vector<double>> get_azimuth_db(unsigned int index, matvar_t* table);

public:
    explicit rcs_data(const string& path);

    [[nodiscard]] vector<double> rcs() const {
        return _rcs_dbs;
    }

    [[nodiscard]] map<long, vector<double>> azimuth() const {
        return _azimuth_db;
    }
};