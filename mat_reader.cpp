#include "mat_reader.h"

static matvar_t* get_table(mat_t* file) {
    matvar_t* matvar = Mat_VarReadInfo(file, "result");
    if (nullptr == matvar) {
        throw runtime_error("Could not read variable 'result'");
    }
    Mat_VarReadDataAll(file, matvar);
    return matvar;
}

static matvar_t* get_variable(unsigned int index,
                              const char* variable,
                              matvar_t* table) {
    auto var = Mat_VarGetStructFieldByName(table, variable, index);
    return var;
}

static unsigned int get_row_index_for_height(unsigned int height, matvar_t* table) {
    auto index = 0;
    const auto name = "height";

    matvar_t* cell = get_variable(index, name, table);
    while (nullptr != cell) {
        auto content = *reinterpret_cast<double*>(cell->data);
        if (content == height) {
            return index;
        }
        cell = get_variable(++index, name, table);
    }

    throw invalid_argument(name);
}

static vector<double> get_raw_values(unsigned int row_index,
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

static vector<double> get_rcs(unsigned int index, matvar_t* table) {
    return get_raw_values(index, "rcs", table);
}

static vector<double> get_rcs_db(unsigned int index, matvar_t* table) {
    return get_raw_values(index, "rcs_dB", table);
}

static vector<long> get_angles(unsigned int index, matvar_t* table) {
    auto raw_values = get_raw_values(index, "angle", table);
    return map_vec<double, long>(raw_values, [](double angle) {
        return static_cast<long>(angle);
    });
}

/**
 * get measurement ranges in centimeters
 */
static vector<long> get_ranges(unsigned int index, matvar_t* table) {
    auto raw_values = get_raw_values(index, "range", table);

    return map_vec<double, long>(raw_values, [](double range) {
        return std::lround(range * 100);
    });
}

map<long, vector<double>> rcs_data::reconstruct_azimuth_table(const vector<double>& raw_values) {
    map<long, vector<double>> result;

    auto angles = _angles.size();
    auto ranges = _ranges.size();

    for (auto angle_i = 0; angle_i < angles; angle_i++) {
        const auto angle = _angles[angle_i];

        vector<double> values;
        values.resize(ranges);
        for (int i = 0; i < ranges; i++) {
            values[i] = raw_values[angle_i * ranges + i];
        }
        result.insert(std::pair<long, vector<double>>(angle, values));
    }
    return result;
}

map<long, vector<double>> rcs_data::get_azimuth(unsigned int index, matvar_t* table) {
    auto raw_values = get_raw_values(index, "AzimuthLeistung", table);
    return reconstruct_azimuth_table(raw_values);
}

map<long, vector<double>> rcs_data::get_azimuth_db(unsigned int index, matvar_t* table) {
    auto raw_values = get_raw_values(index, "AzimuthLeistung_dB", table);
    return reconstruct_azimuth_table(raw_values);
}

rcs_data::rcs_data(const string& path) {
    mat_t* mat_file_handle = Mat_Open(path.c_str(), MAT_ACC_RDONLY);
    if (nullptr == mat_file_handle) {
        throw runtime_error("Could not open .mat file");
    }

    auto table = get_table(mat_file_handle);
    auto row_index = get_row_index_for_height(40, table);

    _rcs = get_rcs(row_index, table);
    _rcs_dbs = get_rcs_db(row_index, table);
    _angles = get_angles(row_index, table);
    _ranges = get_ranges(row_index, table);
    _azimuth = get_azimuth(row_index, table);
    _azimuth_db = get_azimuth_db(row_index, table);

    Mat_Close(mat_file_handle);
}
