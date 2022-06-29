#ifndef RCSOP_DATA_RCS_DATA_UTILS_H
#define RCSOP_DATA_RCS_DATA_UTILS_H

#include <regex>
#include <execution>
#include <mutex>

#include "utils/types.h"
#include "utils/chronometer.h"

#include "observer_position.h"
#include "az_data.h"
#include "input_image.h"
#include "texture.h"

using std::filesystem::recursive_directory_iterator;
using std::regex;
using std::smatch;

struct mat_path_with_azimuth {
    path file_path;
    azimuth_t azimuth;
};

vector<mat_path_with_azimuth> find_mat_files_in_folder(const path& folder_for_height,
                                                       const regex& data_file_regex);

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

    auto file_paths = find_mat_files_in_folder(height_path, data_file_regex);
    std::mutex map_mutex;
    std::for_each(std::execution::par_unseq,
                  begin(file_paths), end(file_paths),
                  [&map_mutex, &azimuth_to_data, &height, &data_file_regex]
                          (const auto& mat_file_entry) {
                      const auto data = make_shared<AT>(
                              mat_file_entry.file_path,
                              ObserverPosition{
                                      .height = height,
                                      .azimuth = mat_file_entry.azimuth,
                              });

                      const std::lock_guard<std::mutex> lock(map_mutex);
                      azimuth_to_data.insert(make_pair(mat_file_entry.azimuth, data));
                  });
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
