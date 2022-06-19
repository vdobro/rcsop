#include "azimuth_rcs_map.h"

#include "utils/rcs_data_utils.h"

#include <regex>
using std::regex;

static const regex mat_file_regex("^DataAuswertung_\\d{1,2}cm_(\\d{1,3})°\\.mat$");

AzimuthRcsMap::AzimuthRcsMap(const path& input_path) {
    this->_data = collect_all_heights<AzimuthInput::RCS_MAT>(input_path, mat_file_regex);
}

map<azimuth_t, shared_ptr<AzimuthRcsDataSet>> AzimuthRcsMap::at_height(height_t height) const {
    return this->_data->at(height);
}

shared_ptr<AzimuthRcsDataSet> AzimuthRcsMap::at_position(const ObserverPosition& position) const {
    return this->at_height(position.height).at(position.azimuth);
}

vector<height_t> AzimuthRcsMap::heights() const {
    vector<height_t> result;
    for (auto const& pair: *this->_data) {
        result.push_back(pair.first);
    }
    return result;
}
