#include "azimuth_rcs_data_collection.h"

#include <regex>
#include <filesystem>

#include "utils/rcs_data_utils.h"

namespace rcsop::data {
    using std::regex;
    using std::smatch;
    using std::filesystem::recursive_directory_iterator;

    using rcsop::data::utils::collect_all_heights;
    using rcsop::data::utils::AzimuthInput;

    static const regex mat_file_regex("^DataAuswertung_(\\d{1,2})cm_(\\d{1,3})°\\.mat$");
    static const size_t FILENAME_HEIGHT_INDEX = 1;
    static const size_t FILENAME_AZIMUTH_INDEX = 2;

    AzimuthRcsDataCollection::AzimuthRcsDataCollection(const path& input_path) {
        this->_data = collect_all_heights<AzimuthInput::RCS_MAT>(
                input_path, mat_file_regex, FILENAME_HEIGHT_INDEX, FILENAME_AZIMUTH_INDEX);

        this->_heights.clear();
        for (auto& [height, data]: _data) {
            this->_heights.push_back(height);
        }
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

    bool AzimuthRcsDataCollection::is_available_at(const path& root_path) {
        auto file_iterator = recursive_directory_iterator{root_path};
        for (const auto& dir_entry: file_iterator) {
            smatch sm;
            const path& file_path = dir_entry.path();
            const string filename = file_path.filename().string();
            if (regex_match(filename, sm, mat_file_regex)) {
                return true;
            }
        }
        return false;
    }
}
