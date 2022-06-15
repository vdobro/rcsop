#ifndef RCSOP_DATA_RCS_DATA_H
#define RCSOP_DATA_RCS_DATA_H

#include <map>
#include <stdexcept>
#include <memory>
#include <string>
#include <cmath>

#include "matio.h"

#include "utils/types.h"

using std::vector;

using std::map;
using std::shared_ptr;
using std::string;
using std::runtime_error;
using std::invalid_argument;

class rcs_data_row {
private:
    vector<double> _rcs;
    vector<double> _rcs_dbs;
    vector<long> _angles;
    vector<long> _ranges;
    map<long, vector<double>> _azimuth;
    map<long, vector<double>> _azimuth_db;

    rcs_data_row() = default;

    map<long, vector<double>> reconstruct_azimuth_table(const vector<double>& raw_values);

    map<long, vector<double>> get_azimuth(size_t index, matvar_t* table);

    map<long, vector<double>> get_azimuth_db(size_t index, matvar_t* table);

public:
    explicit rcs_data_row(size_t row_index, matvar_t* table);

    [[nodiscard]] vector<double> rcs() const;

    [[nodiscard]] map<long, vector<double>> azimuth() const;

    [[nodiscard]] vector<long> ranges() const;

    [[nodiscard]] vector<long> angles() const;
};

class rcs_data {
private:
    map<long, shared_ptr<rcs_data_row>> _rows;
public:
    explicit rcs_data(const path& path);

    [[nodiscard]] shared_ptr<rcs_data_row> at_height(long height) const;

    [[nodiscard]] vector<long> available_heights() const;
};

#endif //RCSOP_DATA_RCS_DATA_H
