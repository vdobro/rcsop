#include "read_rcs.h"
#include "utils.h"

#include <stdexcept>
#include <cmath>

using std::vector;

mat_t* open_mat_file(const std::string& path) {
    mat_t* matfp = Mat_Open(path.c_str(), MAT_ACC_RDONLY);

    if (nullptr == matfp) {
        throw std::runtime_error("Could not open .mat file");
    }

    return matfp;
}

void close_mat_file(mat_t* file_handle) {
    if (nullptr == file_handle) {
        throw std::invalid_argument(".mat file already closed");
    }
    Mat_Close(file_handle);
}


matvar_t* get_table(mat_t* file) {
    matvar_t* matvar = Mat_VarReadInfo(file, "result");
    if (nullptr == matvar) {
        throw std::runtime_error("Could not read variable 'result'");
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

unsigned int get_row_for_height(unsigned int height, matvar_t* table) {
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

    throw std::invalid_argument(name);
}

static vector<double> get_raw_values(unsigned int index, const char* name,
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

vector<double> get_rcs(unsigned int index, matvar_t* table) {
    return get_raw_values(index, "rcs", table);
}

vector<double> get_rcs_db(unsigned int index, matvar_t* table) {
    return get_raw_values(index, "rcs_dB", table);
}

vector<long> get_angles(unsigned int index, matvar_t* table) {
    auto raw_values = get_raw_values(index, "angle", table);
    return map_vec<double, long>(raw_values, [](double angle) {
        return static_cast<long>(angle);
    });
}

/**
 * get measurement ranges in centimeters
 */
vector<long> get_ranges(unsigned int index, matvar_t* table) {
    auto raw_values = get_raw_values(index, "range", table);

    return map_vec<double, long>(raw_values, [](double range) {
        return std::lround(range * 100);
    });
}