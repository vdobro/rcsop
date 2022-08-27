#include "azimuth_minimap_provider.h"

#include <regex>
#include <filesystem>

#include "utils/rcs_data_utils.h"

namespace rcsop::data {
    using rcsop::data::utils::AzimuthInput;
    using rcsop::data::utils::collect_all_heights;

    static const regex minimap_filename_pattern("^figure_(\\d{1,2})cm_(\\d{1,3})°_RangevsAzimuth\\.png$");
    static const size_t FILENAME_HEIGHT_INDEX = 1;
    static const size_t FILENAME_AZIMUTH_INDEX = 2;

    AzimuthMinimapProvider::AzimuthMinimapProvider(const path& input_root_path) {
        this->_data = collect_all_heights<AzimuthInput::MINIMAP>(
                input_root_path, minimap_filename_pattern, FILENAME_HEIGHT_INDEX, FILENAME_AZIMUTH_INDEX);
    }

    auto AzimuthMinimapProvider::for_position(const Observer& observer) const -> const Texture& {
        const auto position = observer.position();
        return this->_data.at(position.height).at(position.azimuth);
    }

    bool AzimuthMinimapProvider::is_available_at(const path& root_path) {
        auto file_iterator = recursive_directory_iterator{root_path};
        for (const auto& dir_entry: file_iterator) {
            smatch sm;
            const path& file_path = dir_entry.path();
            const string filename = file_path.filename().string();
            if (regex_match(filename, sm, minimap_filename_pattern)) {
                return true;
            }
        }
        return false;
    }
}
