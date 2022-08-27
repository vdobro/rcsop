#include "azimuth_rcs_data_collection.h"

#include <regex>
#include <filesystem>

#include "utils/rcs_data_utils.h"

namespace rcsop::data {
    using rcsop::data::utils::collect_all_heights;
    using rcsop::data::utils::AzimuthInput;

    static const regex mat_file_regex("^DataAuswertung_(\\d{1,2})cm_(\\d{1,3})°\\.mat$");
    static const size_t FILENAME_HEIGHT_INDEX = 1;
    static const size_t FILENAME_AZIMUTH_INDEX = 2;

    AzimuthRcsDataCollection::AzimuthRcsDataCollection(path input_path) : _root_path(input_path) {
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
        const auto& height = position.height;
        const auto& azimuth = position.azimuth;

        if (!_data.contains(height)) {
            throw std::domain_error("No data set at " + std::to_string(height) + "cm in " + _root_path.string());
        }
        auto& map_to_azimuth = _data.at(position.height);
        if (!map_to_azimuth.contains(azimuth)) {
            throw std::domain_error(
                    "No data set at " + std::to_string(height) + "cm/" + std::to_string(azimuth) + " in " +
                    _root_path.string());
        }
        return &(map_to_azimuth.at(azimuth));
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

    vector<ObserverPosition> AzimuthRcsDataCollection::available_positions() const {
        vector<ObserverPosition> result;
        for (auto height: heights()) {
            auto& map_to_azimuth = _data.at(height);
            for (const auto& [azimuth, data]: map_to_azimuth) {
                result.push_back(
                        {
                                .height = height,
                                .azimuth = azimuth,
                        });
            }
        }
        return result;
    }
}
