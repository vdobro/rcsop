#include "az_data.h"

#include <numeric>

#include "utils/mapping.h"

namespace rcsop::data {
    using std::reduce;
    using std::nanf;

    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::find_nearest;
    using rcsop::common::utils::find_nearest_index;

    const char* VARIABLE_MAIN = "auswertung";
    const char* TABLE_RANGES = "vRangeExt";
    const char* TABLE_ANGLES = "vAngDeg";
    const char* TABLE_AZIMUTH_VALUES = "JOpt_RCS";

    static matvar_t* get_all_variables(mat_t* file) {
        matvar_t* matvar = Mat_VarReadInfo(file, VARIABLE_MAIN);
        if (nullptr == matvar) {
            throw runtime_error("Could not read variable 'result'");
        }
        Mat_VarReadDataAll(file, matvar);
        return matvar;
    }

    static matvar_t* get_table_field(const char* name, matvar_t* table) {
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

    az_value_map_t AzimuthRcsDataSet::reconstruct_value_table(const vector<double>& raw_values) {
        az_value_map_t result;

        auto angles = _angles.size();
        auto ranges = _ranges.size();

        for (size_t angle_i = 0; angle_i < angles; angle_i++) {
            const rcs_angle_t angle = _angles[angle_i];

            vector<double> range_values;
            range_values.resize(ranges);
            for (size_t i = 0; i < ranges; i++) {
                range_values[i] = raw_values[angle_i * ranges + i];
            }
            range_values.erase(range_values.begin()); // first value is NaN because of the range = 0
            result.insert(make_pair(angle, range_values));
        }
        return result;
    }

    void AzimuthRcsDataSet::filter_peaks() {
        for (const auto& [angle, raw_column]: _raw_values) {
            double max_value = rcsop::common::utils::max_value(raw_column);

            vector<double> filtered_column = map_vec<double, double>(raw_column, [&max_value]
                    (const double& value) -> double {
                if (value < max_value) {
                    return 0;
                }
                return value;
            });
            _filtered_values.insert(make_pair(angle, filtered_column));
        }
    }

    AzimuthRcsDataSet::AzimuthRcsDataSet(const path& filename,
                                         const ObserverPosition& position) {
        mat_t* mat_file_handle = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
        if (nullptr == mat_file_handle) {
            throw runtime_error("Could not open .mat file");
        }
        auto table = get_all_variables(mat_file_handle);
        _ranges = map_vec<double, rcs_distance_t>(get_raw_values(TABLE_RANGES, table), [](double range) {
            return std::lround(range * 100);
        });

        _angles = get_raw_values(TABLE_ANGLES, table);
        auto raw_azimuth = get_raw_values(TABLE_AZIMUTH_VALUES, table);
        _raw_values = reconstruct_value_table(raw_azimuth);

        Mat_VarFree(table);
        Mat_Close(mat_file_handle);

        _ranges.erase(_ranges.begin()); // first row contains only NaN
        _last_range_index = _ranges.size() - 1;
        _last_range_step = abs(_ranges[_last_range_index] - _ranges[_last_range_index - 1]);
        filter_peaks();
    }

    rcs_value_t AzimuthRcsDataSet::resolve_value(rcs_angle_t angle,
                                                 size_t range_index) const {
        const auto& value_map = (use_filtered) ? _filtered_values : _raw_values;
        const auto& range_to_values = value_map.at(angle);
        return range_to_values[range_index];
    }

    double AzimuthRcsDataSet::map_to_nearest(const observed_point& point) const {
        const long range_distance = lround(point.distance_in_world);

        const rcs_angle_t nearest_angle = find_nearest(point.horizontal_angle, _angles);
        const size_t nearest_range_index = find_nearest_index(range_distance, _ranges);

        const auto is_last = nearest_range_index == _last_range_index;
        const auto last_range = _ranges[_last_range_index];
        const auto out_of_range = (is_last && abs(last_range - range_distance) > _last_range_step);

        return out_of_range ? nanf("Distance out of range.") : resolve_value(nearest_angle, nearest_range_index);
    }

    void AzimuthRcsDataSet::use_filtered_peaks() {
        this->use_filtered = true;
    }

    rcs_value_t AzimuthRcsDataSet::map_exact(rcs_distance_t distance, rcs_angle_t angle) const {
        const size_t nearest_range_index = find_nearest_index(distance, _ranges);
        return resolve_value(angle, nearest_range_index);
    }

    vector<rcs_distance_t> AzimuthRcsDataSet::distances() const {
        return this->_ranges;
    }

    vector<rcs_angle_t> AzimuthRcsDataSet::angles() const {
        return this->_angles;
    }

    rcs_distance_t AzimuthRcsDataSet::distance_step() const {
        return this->_last_range_step;
    }
}
