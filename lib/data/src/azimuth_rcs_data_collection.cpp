#include "azimuth_rcs_data_collection.h"

#include <regex>

#include "utils/rcs_data_utils.h"

namespace rcsop::data {
    using std::regex;
    using rcsop::data::utils::collect_all_heights;
    using rcsop::data::utils::AzimuthInput;

    static const regex mat_file_regex("^DataAuswertung_\\d{1,2}cm_(\\d{1,3})°\\.mat$");

    AzimuthRcsDataCollection::AzimuthRcsDataCollection(const path& input_path) {
        this->_data = collect_all_heights<AzimuthInput::RCS_MAT>(input_path, mat_file_regex);

        this->_heights.clear();
        for (auto& [height, data]: _data) {
            this->_heights.push_back(height);
        }
        //this->_heights = vector<height_t>(data_keys.begin(), data_keys.end());
    }

    const AbstractDataSet* AzimuthRcsDataCollection::get_for_exact_position(
            const Observer& observer) const {
        const auto position = observer.position();
        return &(this->_data.at(position.height).at(position.azimuth));
    }

    void AzimuthRcsDataCollection::use_filtered_peaks() {
        for (auto& [height, azimuth_data]: _data) {
            for (auto& [azimuth, data]: azimuth_data) {
                data.use_filtered_peaks();
            }
        }
    }

    vector<height_t> AzimuthRcsDataCollection::heights() const {
        return this->_heights;
    }
}
