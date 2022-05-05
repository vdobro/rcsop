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

static unsigned int get_row_for_height(unsigned int height, matvar_t* table) {
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

static vector<double> get_raw_values(unsigned int index,
                                     const char* name,
                                     matvar_t* table) {
    auto rcs = get_variable(index, name, table);

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

rcs_data::rcs_data(const string& path) {
    mat_t* mat_file_handle = Mat_Open(path.c_str(), MAT_ACC_RDONLY);
    if (nullptr == mat_file_handle) {
        throw runtime_error("Could not open .mat file");
    }

    auto table = get_table(mat_file_handle);
    auto index = get_row_for_height(40, table);

    _rcs = get_rcs(index, table);
    _rcs_dbs = get_rcs_db(index, table);
    _angles = get_angles(index, table);
    _ranges = get_ranges(index, table);

    Mat_Close(mat_file_handle);
}
