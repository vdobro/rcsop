#include "azimuth_rcs_data_collection.h"

#include "utils/rcs_data_utils.h"

#include <regex>

using std::regex;

static const regex mat_file_regex("^DataAuswertung_\\d{1,2}cm_(\\d{1,3})°\\.mat$");

AzimuthRcsDataCollection::AzimuthRcsDataCollection(const path& input_path) {
    this->_data = collect_all_heights<AzimuthInput::RCS_MAT>(input_path, mat_file_regex);
}

shared_ptr<AbstractDataSet> AzimuthRcsDataCollection::at_position(const ObserverPosition& position) const {
    return std::static_pointer_cast<AbstractDataSet>(
            this->_data->at(position.height).at(position.azimuth));
}
