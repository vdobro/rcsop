#include "basic_rcs_map.h"

#include <cmath>

#include "utils/mapping.h"

namespace rcsop::data {
    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::cast_vec;

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

    static map<height_t, size_t> get_heights(matvar_t* table) {
        map<height_t, size_t> result;
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

        auto* rcs_doubles = reinterpret_cast<double*>(rcs->data);

        vector<double> raw_values;
        raw_values.resize(rcs->nbytes / rcs->data_size);
        for (size_t i = 0; i < raw_values.size(); i++) {
            raw_values[i] = rcs_doubles[i];
        }
        return raw_values;
    }

    static vector<rcs_t> get_rcs(size_t index, matvar_t* table) {
        return get_raw_values(index, RCS_COLUMN_RCS, table);
    }

    static vector<rcs_t> get_rcs_db(size_t index, matvar_t* table) {
        return get_raw_values(index, RCS_COLUMN_RCS_DB, table);
    }

    static vector<angle_t> get_angles(size_t index, matvar_t* table) {
        auto raw_values = get_raw_values(index, RCS_COLUMN_ANGLE, table);

        return cast_vec<double, angle_t, true>(raw_values);
    }

    /**
     * get measurement ranges in centimeters
     */
    static vector<range_t> get_ranges(size_t index, matvar_t* table) {
        auto raw_values = get_raw_values(index, RCS_COLUMN_RANGE, table);

        return map_vec<double, range_t>(raw_values, [](double range) -> long {
            return std::lround(range * 100);
        });
    }

    map<angle_t, vector<rcs_t>> BasicRcsDataSet::reconstruct_azimuth_table(const vector<double>& raw_values) {
        map<angle_t, vector<rcs_t>> result;

        auto angles = _angles.size();
        auto ranges = _ranges.size();

        for (size_t angle_i = 0; angle_i < angles; angle_i++) {
            const angle_t angle = _angles[angle_i];

            vector<rcs_t> values;
            values.resize(ranges);
            for (size_t i = 0; i < ranges; i++) {
                values[i] = raw_values[angle_i * ranges + i];
            }
            result.insert(make_pair(angle, values));
        }
        return result;
    }

    map<angle_t, vector<rcs_t>> BasicRcsDataSet::get_azimuth(size_t index, matvar_t* table) {
        auto raw_values = get_raw_values(index, RCS_COLUMN_AZIMUTH, table);
        return reconstruct_azimuth_table(raw_values);
    }

    map<angle_t, vector<rcs_t>> BasicRcsDataSet::get_azimuth_db(size_t index, matvar_t* table) {
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

    map<angle_t, vector<double>> BasicRcsDataSet::azimuth() const {
        return _azimuth;
    }

    vector<range_t> BasicRcsDataSet::ranges() const {
        return _ranges;
    }

    vector<angle_t> BasicRcsDataSet::angles() const {
        return this->_angles;
    }

    BasicRcsMap::BasicRcsMap(const path& path) {
        mat_t* mat_file_handle = Mat_Open(path.c_str(), MAT_ACC_RDONLY);
        if (nullptr == mat_file_handle) {
            throw runtime_error("Could not open .mat file");
        }

        auto table = get_table(mat_file_handle);
        auto height_row_indices = get_heights(table);

        for (auto& [height, index]: height_row_indices) {
            auto rcs = make_shared<BasicRcsDataSet>(BasicRcsDataSet(index, table));
            this->_rows.insert(make_pair(height, rcs));
        }

        Mat_VarFree(table);
        Mat_Close(mat_file_handle);
    }

    shared_ptr<BasicRcsDataSet> BasicRcsMap::at_height(height_t height) const {
        return _rows.at(height);
    }

    vector<height_t> BasicRcsMap::available_heights() const {
        vector<height_t> heights;
        for (auto& [row, _]: _rows) {
            heights.push_back(row);
        }
        std::sort(heights.begin(), heights.end());
        return heights;
    }

    bool BasicRcsMap::is_available_at(const path& file_path) {
        return std::filesystem::is_regular_file(file_path);
    }
}
