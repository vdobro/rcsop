#include "utils/rcs_data_utils.h"

namespace rcsop::data::utils {
    using std::filesystem::directory_iterator;

    vector<mat_path_with_azimuth> find_mat_files_in_folder(
            const path& folder_for_height,
            const regex& data_filename_pattern,
            size_t pattern_azimuth_index) {

        auto file_iterator = recursive_directory_iterator{folder_for_height};
        vector<mat_path_with_azimuth> paths;
        for (const auto& dir_entry: file_iterator) {
            smatch sm;
            const path& file_path = dir_entry.path();
            const string filename = file_path.filename().string();
            if (!regex_match(filename, sm, data_filename_pattern)) {
                continue;
            }
            const azimuth_t azimuth = std::stoi(sm[pattern_azimuth_index]);
            paths.push_back(mat_path_with_azimuth{
                    .file_path = file_path,
                    .azimuth = azimuth,
            });
        }

        return paths;
    }

    vector<height_data_folder> parse_available_heights(
            const path& data_path,
            const regex& data_filename_pattern,
            size_t pattern_height_index) {
        vector<height_data_folder> result;
        std::set<height_t> heights;
        for (auto const& dir_entry: recursive_directory_iterator{data_path}) {
            if (dir_entry.is_directory()) {
                continue;
            }
            smatch sm;
            auto& path = dir_entry.path();
            auto file_name = path.filename().string();
            if (!regex_match(file_name, sm, data_filename_pattern)) {
                continue;
            }

            height_t height = stoi(sm[pattern_height_index]);
            if (heights.contains(height)) {
                continue;
            }
            heights.insert(height);

            height_data_folder height_entry{
                    .folder_path = path.parent_path(),
                    .height = height,
            };
            result.push_back(height_entry);
        }
        return result;
    }
}
