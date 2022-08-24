#ifndef RCSOP_DATA_RCS_DATA_H
#define RCSOP_DATA_RCS_DATA_H

#include "matio.h"

#include "utils/types.h"
#include "observer_position.h"

namespace rcsop::data {

    using rcsop::common::height_t;
    using range_t = long;
    using angle_t = long;
    using rcs_t = double;

    class BasicRcsDataSet {
    private:
        vector<rcs_t> _rcs;
        vector<rcs_t> _rcs_dbs;

        vector<angle_t> _angles;
        vector<range_t> _ranges;

        map<angle_t, vector<rcs_t>> _azimuth;
        map<angle_t, vector<rcs_t>> _azimuth_db;

        BasicRcsDataSet() = default;

        map<angle_t, vector<rcs_t>> reconstruct_azimuth_table(const vector<double>& raw_values);

        map<angle_t, vector<rcs_t>> get_azimuth(size_t index, matvar_t* table);

        map<angle_t, vector<rcs_t>> get_azimuth_db(size_t index, matvar_t* table);

    public:
        explicit BasicRcsDataSet(size_t row_index, matvar_t* table);

        [[nodiscard]] vector<double> rcs() const;

        [[nodiscard]] map<angle_t, vector<double>> azimuth() const;

        [[nodiscard]] vector<range_t> ranges() const;

        [[nodiscard]] vector<angle_t> angles() const;
    };

    class BasicRcsMap {
    private:
        map<height_t, shared_ptr<BasicRcsDataSet>> _rows;
    public:
        explicit BasicRcsMap(const path& path);

        [[nodiscard]] shared_ptr<BasicRcsDataSet> at_height(height_t height) const;

        [[nodiscard]] vector<height_t> available_heights() const;

        [[nodiscard]] static bool is_available_at(const path& file_path);
    };
}

#endif //RCSOP_DATA_RCS_DATA_H
