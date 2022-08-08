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

namespace rcsop::data::utils {
    using std::filesystem::recursive_directory_iterator;
    using std::regex;
    using std::smatch;

    using rcsop::common::height_t;
    using rcsop::common::azimuth_t;

    using rcsop::common::ObserverPosition;

    using rcsop::common::utils::time::start_time;
    using rcsop::common::utils::time::log_and_start_next;

    struct mat_path_with_azimuth {
        path file_path;
        azimuth_t azimuth;
    };

    struct height_data_folder {
        path folder_path;
        height_t height;
    };

    vector<mat_path_with_azimuth> find_mat_files_in_folder(
            const path& folder_for_height,
            const regex& data_file_regex);

    vector<height_data_folder> parse_available_heights(const path& data_path);

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
        using type = rcsop::common::Texture;
    };

    template<AzimuthInput T>
    using AzimuthInputType = typename AzimuthAssetTrait<T>::type;

    template<AzimuthInput T>
    map<azimuth_t, AzimuthInputType<T>> map_azimuth_angles_to_data(
            const height_data_folder& height_folder,
            const regex& data_file_regex) {
        using AT = AzimuthInputType<T>;

        const height_t height = height_folder.height;

        map<azimuth_t, AT> azimuth_to_data;

        auto file_paths = find_mat_files_in_folder(height_folder.folder_path, data_file_regex);
        std::mutex map_mutex;
        std::for_each(std::execution::par_unseq,
                      begin(file_paths), end(file_paths),
                      [&map_mutex, &azimuth_to_data, &height](const mat_path_with_azimuth& mat_file_entry) {
                          auto data = AT(
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
    map<height_t, map<azimuth_t, AzimuthInputType<T>>> collect_all_heights(
            const path& data_path,
            const regex& data_filename_pattern) {
        using AT = AzimuthInputType<T>;
        using azimuth_map_t = map<azimuth_t, AT>;
        using result_map_t = map<height_t, azimuth_map_t>;

        auto start = start_time();
        auto heights = parse_available_heights(data_path);
        result_map_t result;
        for (auto height_folder: heights) {
            azimuth_map_t angle_mapping = map_azimuth_angles_to_data<T>(height_folder, data_filename_pattern);
            result.insert(make_pair(height_folder.height, angle_mapping));
        }
        log_and_start_next(start, "Reading " + string(azimuthInputTypeDescriptions[T]) + " finished");
        return result;
    }
}

#endif //RCSOP_RCS_DATA_UTILS_H
