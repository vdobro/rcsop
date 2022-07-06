#include "azimuth_minimap_provider.h"

#include "utils/rcs_data_utils.h"

#include <regex>
using std::regex;

static const regex minimap_filename_pattern("^figure_\\d{1,2}cm_(\\d{1,3})°_RangevsAzimuth\\.png$");

AzimuthMinimapProvider::AzimuthMinimapProvider(const path& input_root_path) {
    this->_data = collect_all_heights<AzimuthInput::MINIMAP>(
            input_root_path,
            minimap_filename_pattern);
}

const Texture& AzimuthMinimapProvider::for_position(const Observer& observer) const {
    const auto position = observer.position();
    return this->_data.at(position.height).at(position.azimuth);
}