#include "basic_rcs_map.h"

#include <stdexcept>
#include <cmath>

#include "utils/mapping.h"

static const auto RCS_TABLE_NAME = "result";
static const auto RCS_COLUMN_HEIGHT = "height";
static const auto RCS_COLUMN_RCS = "rcs";
static const auto RCS_COLUMN_RCS_DB = "rcs_dB";
static const auto RCS_COLUMN_RANGE = "range";
static const auto RCS_COLUMN_ANGLE = "angle";
static const auto RCS_COLUMN_AZIMUTH = "AzimuthLeistung";
static const auto RCS_COLUMN_AZIMUTH_DB = "AzimuthLeistung_dB";

static matvar_t* get_table(mat_t* file) {
    matvar_t* matvar = Mat_VarReadInfo(file, RCS_TABLE_NAME);
    if (nullptr == matvar) {
        throw runtime_error("Could not read variable 'result'");
    }
    Mat_VarReadDataAll(file, matvar);
    return matvar;
}

static matvar_t* get_variable(size_t index,
                              const char* variable,
                              matvar_t* table) {
    auto var = Mat_VarGetStructFieldByName(table, variable, index);
    return var;
}

static map<long, size_t> get_heights(matvar_t* table) {
    map<long, size_t> result;
    auto index = 0;
    auto name = RCS_COLUMN_HEIGHT;

    matvar_t* cell = get_variable(index, name, table);
    while (nullptr != cell) {
        auto height_raw = *reinterpret_cast<double*>(cell->data);
        auto height = lround(height_raw);
        result.insert(make_pair(height, index));

        cell = get_variable(++index, name, table);
    }
    return result;
}

static vector<double> get_raw_values(size_t row_index,
                                     const char* name,
                                     matvar_t* table) {
    auto rcs = get_variable(row_index, name, table);

    auto rcs_doubles = reinterpret_cast<double*>(rcs->data);

    vector<double> raw_values;
    raw_values.resize(rcs->nbytes / rcs->data_size);
    for (size_t i = 0; i < raw_values.size(); i++) {
        raw_values[i] = rcs_doubles[i];
    }
    return raw_values;
}

static vector<double> get_rcs(size_t index, matvar_t* table) {
    return get_raw_values(index, RCS_COLUMN_RCS, table);
}

static vector<double> get_rcs_db(size_t index, matvar_t* table) {
    return get_raw_values(index, RCS_COLUMN_RCS_DB, table);
}

static vector<long> get_angles(size_t index, matvar_t* table) {
    auto raw_values = get_raw_values(index, RCS_COLUMN_ANGLE, table);

    return cast_vec<double, long>(raw_values);
}

/**
 * get measurement ranges in centimeters
 */
static vector<long> get_ranges(size_t index, matvar_t* table) {
    auto raw_values = get_raw_values(index, RCS_COLUMN_RANGE, table);

    return map_vec<double, long>(raw_values, [](double range) -> long {
        return std::lround(range * 100);
    });
}

map<long, vector<double>> BasicRcsDataSet::reconstruct_azimuth_table(const vector<double>& raw_values) {
    map<long, vector<double>> result;

    auto angles = _angles.size();
    auto ranges = _ranges.size();

    for (size_t angle_i = 0; angle_i < angles; angle_i++) {
        const auto angle = _angles[angle_i];

        vector<double> values;
        values.resize(ranges);
        for (size_t i = 0; i < ranges; i++) {
            values[i] = raw_values[angle_i * ranges + i];
        }
        result.insert(make_pair(angle, values));
    }
    return result;
}

map<long, vector<double>> BasicRcsDataSet::get_azimuth(size_t index, matvar_t* table) {
    auto raw_values = get_raw_values(index, RCS_COLUMN_AZIMUTH, table);
    return reconstruct_azimuth_table(raw_values);
}

map<long, vector<double>> BasicRcsDataSet::get_azimuth_db(size_t index, matvar_t* table) {
    auto raw_values = get_raw_values(index, RCS_COLUMN_AZIMUTH_DB, table);
    return reconstruct_azimuth_table(raw_values);
}

BasicRcsDataSet::BasicRcsDataSet(size_t row_index, matvar_t* table) {
    _rcs = get_rcs(row_index, table);
    _rcs_dbs = get_rcs_db(row_index, table);
    _angles = get_angles(row_index, table);
    _ranges = get_ranges(row_index, table);
    _azimuth = get_azimuth(row_index, table);
    _azimuth_db = get_azimuth_db(row_index, table);
}

vector<double> BasicRcsDataSet::rcs() const {
    return _rcs;
}

map<long, vector<double>> BasicRcsDataSet::azimuth() const {
    return _azimuth;
}

vector<long> BasicRcsDataSet::ranges() const {
    return _ranges;
}

vector<long> BasicRcsDataSet::angles() const {
    return this->_angles;
}

BasicRcsMap::BasicRcsMap(const path& path) {
    mat_t* mat_file_handle = Mat_Open(path.c_str(), MAT_ACC_RDONLY);
    if (nullptr == mat_file_handle) {
        throw runtime_error("Could not open .mat file");
    }

    auto table = get_table(mat_file_handle);
    auto height_row_indices = get_heights(table);

    for (auto& height_row_index: height_row_indices) {
        auto height = height_row_index.first;
        auto index = height_row_index.second;

        auto rcs = make_shared<BasicRcsDataSet>(BasicRcsDataSet(index, table));
        this->_rows.insert(make_pair(height, rcs));
    }

    Mat_VarFree(table);
    Mat_Close(mat_file_handle);
}

shared_ptr<BasicRcsDataSet> BasicRcsMap::at_height(long height) const {
    return _rows.at(height);
}

vector<long> BasicRcsMap::available_heights() const {
    vector<long> heights;
    for (auto& row: _rows) {
        heights.push_back(row.first);
    }
    std::sort(heights.begin(), heights.end());
    return heights;
}
