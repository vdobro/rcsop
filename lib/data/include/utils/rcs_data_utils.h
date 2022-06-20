#ifndef RCSOP_DATA_RCS_DATA_UTILS_H
#define RCSOP_DATA_RCS_DATA_UTILS_H

#include <regex>

#include "utils/types.h"
#include "observer_position.h"
#include "utils/chronometer.h"
#include "az_data.h"
#include "input_image.h"
#include "texture.h"

using std::filesystem::recursive_directory_iterator;
using std::regex;
using std::smatch;

vector<height_t> parse_available_heights(const path& data_path);

enum AzimuthInput {
    RCS_MAT = 0,
    MINIMAP = 1,
};

static const char* azimuthInputTypeDescriptions[2] = {
        "azimuth-specific RCS .mat data",
        "RCS preview minimap",
};

template<AzimuthInput T>
struct AzimuthAssetTrait {
};

template<>
struct AzimuthAssetTrait<RCS_MAT> {
    using type = AzimuthRcsDataSet;
};

template<>
struct AzimuthAssetTrait<MINIMAP> {
    using type = Texture;
};

template<AzimuthInput T>
using AzimuthInputType = typename AzimuthAssetTrait<T>::type;

template<AzimuthInput T>
map<azimuth_t, shared_ptr<AzimuthInputType<T>>> map_azimuth_angles_to_data(
        height_t height,
        const regex& data_file_regex,
        const path& data_path) {
    using AT = AzimuthInputType<T>;

    auto folder_name = std::to_string(height) + "cm";
    const path height_path{data_path / folder_name};

    map<azimuth_t, shared_ptr<AT>> azimuth_to_data;
    for (auto const& dir_entry: recursive_directory_iterator{height_path}) {
        smatch sm;
        const path& file_path = dir_entry.path();
        const string filename = file_path.filename().string();
        if (!regex_match(filename, sm, data_file_regex)) {
            continue;
        }
        azimuth_t azimuth = stoi(sm[1]);
        ObserverPosition position = {
                .height = height,
                .azimuth = azimuth,
        };
        auto data = make_shared<AT>(file_path, position);
        azimuth_to_data.insert(make_pair(azimuth, data));
    }
    return azimuth_to_data;
}

template<AzimuthInput T>
shared_ptr<map<height_t, map<azimuth_t, shared_ptr<AzimuthInputType<T>>>>> collect_all_heights(
        const path& data_path,
        const regex& data_filename_pattern) {
    using AT = AzimuthInputType<T>;
    using result_map_t = map<height_t, map<azimuth_t, shared_ptr<AT>>>;

    auto start = start_time();
    auto heights = parse_available_heights(data_path);
    shared_ptr<result_map_t> result = make_shared<result_map_t>();
    result->clear();
    for (auto height: heights) {
        map<azimuth_t, shared_ptr<AT>> angle_mapping = map_azimuth_angles_to_data<T>(
                height, data_filename_pattern, data_path);
        result->insert(make_pair(height, angle_mapping));
    }
    log_and_start_next(start, "Reading " + string(azimuthInputTypeDescriptions[T]) + " finished");
    return result;
}

#endif //RCSOP_RCS_DATA_UTILS_H
