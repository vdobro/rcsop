#include "az_data.h"

static matvar_t* get_all_variables(mat_t* file) {
    matvar_t* matvar = Mat_VarReadInfo(file, "auswertung");
    if (nullptr == matvar) {
        throw runtime_error("Could not read variable 'result'");
    }
    Mat_VarReadDataAll(file, matvar);
    return matvar;
}

static matvar_t * get_table_field(const char* name, matvar_t* table) {
    auto var = Mat_VarGetStructFieldByName(table, name, 0);
    return var;
}

static vector<double> get_raw_values(const char* name,
                                     matvar_t* table) {
    auto rcs = get_table_field(name, table);

    auto rcs_doubles = reinterpret_cast<double*>(rcs->data);

    vector<double> raw_values;
    raw_values.resize(rcs->nbytes / rcs->data_size);
    for (size_t i = 0; i < raw_values.size(); i++) {
        raw_values[i] = rcs_doubles[i];
    }
    return raw_values;
}

map<double, vector<double>> az_data::reconstruct_azimuth_table(const vector<double>& raw_values) {
    map<double, vector<double>> result;

    auto angles = _angles.size();
    auto ranges = _ranges.size();

    for (size_t angle_i = 0; angle_i < angles; angle_i++) {
        const auto angle = _angles[angle_i];

        vector<double> range_values;
        range_values.resize(ranges);
        for (size_t i = 0; i < ranges; i++) {
            range_values[i] = raw_values[angle_i * ranges + i];
        }
        result.insert(std::make_pair(angle, range_values));
    }
    return result;
}

az_data::az_data(const std::string& filename) {
    mat_t* mat_file_handle = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
    if (nullptr == mat_file_handle) {
        throw runtime_error("Could not open .mat file");
    }
    auto table = get_all_variables(mat_file_handle);
    _ranges = get_raw_values("vRangeExt", table);
    _angles = get_raw_values("vAngDeg", table);
    auto raw_azimuth = get_raw_values("JOpt_RCS", table);
    _angle_to_rcs_values = reconstruct_azimuth_table(raw_azimuth);

    Mat_Close(mat_file_handle);
}

map<double, vector<double>> az_data::get_rcs() const {
    return this->_angle_to_rcs_values;
}
