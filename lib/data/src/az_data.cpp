#include "az_data.h"

#include <numeric>

#include "utils/mapping.h"

namespace rcsop::data {
    using std::reduce;
    using rcsop::common::utils::map_vec;
    using rcsop::common::utils::find_interval_match;
    using rcsop::common::utils::find_interval_match_index;

    static matvar_t* get_all_variables(mat_t* file) {
        matvar_t* matvar = Mat_VarReadInfo(file, "auswertung");
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

    az_value_map_t AzimuthRcsDataSet::reconstruct_value_table(
            const vector<double>& raw_values) {
        az_value_map_t result;

        auto angles = _angles.size();
        auto ranges = _ranges.size();

        for (size_t angle_i = 0; angle_i < angles; angle_i++) {
            const az_angle_t angle = _angles[angle_i];

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

    const size_t ROWS_TO_SKIP = 1; // first row is filled with NaN

    void AzimuthRcsDataSet::determine_step_sizes() {
        vector<long> range_steps;
        for (size_t i = ROWS_TO_SKIP; i < _ranges.size(); i++) {
            range_steps.push_back(_ranges[i] - _ranges[i - 1]);
        }
        _range_step = reduce(range_steps.begin(), range_steps.end())
                      / static_cast<long>(range_steps.size());

        vector<az_angle_t> angle_steps;
        for (size_t i = ROWS_TO_SKIP; i < _angles.size(); i++) {
            angle_steps.push_back(_angles[i] - _angles[i - 1]);
        }
        _angle_step = reduce(angle_steps.begin(), angle_steps.end())
                      / static_cast<az_angle_t>(angle_steps.size());
    }

    void AzimuthRcsDataSet::filter_peaks() {
        for (const auto& [angle, raw_column]: _raw_values) {
            double max_value = *std::max_element(raw_column.cbegin(), raw_column.cend());

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
                                         const ObserverPosition& position) : _position(position) {
        mat_t* mat_file_handle = Mat_Open(filename.c_str(), MAT_ACC_RDONLY);
        if (nullptr == mat_file_handle) {
            throw runtime_error("Could not open .mat file");
        }
        auto table = get_all_variables(mat_file_handle);
        _ranges = map_vec<double, az_range_t>(get_raw_values("vRangeExt", table), [](double range) {
            return std::lround(range * 100);
        });

        _angles = get_raw_values("vAngDeg", table);
        auto raw_azimuth = get_raw_values("JOpt_RCS", table);
        _raw_values = reconstruct_value_table(raw_azimuth);

        Mat_VarFree(table);
        Mat_Close(mat_file_handle);

        _ranges.erase(_ranges.begin()); // first row contains only NaN
        determine_step_sizes();

        filter_peaks();
    }

    double AzimuthRcsDataSet::map_to_nearest(const observed_point& point) const {
        const double range_distance = point.distance_in_world;
        const double angle = point.horizontal_angle;

        const auto nearest_angle = find_interval_match(angle, _angles, _angles[0], _angle_step / 2);
        const auto nearest_range_index = find_interval_match_index(range_distance, _ranges[0], _range_step / 2);

        const auto& value_map = (use_filtered) ? _filtered_values : _raw_values;
        const auto& range_to_values = value_map.at(nearest_angle);
        if (nearest_range_index >= range_to_values.size()) {
            return std::nanf("out of distance range");
        }
        return range_to_values.at(nearest_range_index);
    }

    void AzimuthRcsDataSet::use_filtered_peaks() {
        this->use_filtered = true;
    }
}
