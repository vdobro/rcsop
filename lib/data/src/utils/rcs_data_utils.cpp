#include "utils/rcs_data_utils.h"

using std::filesystem::directory_iterator;

static const regex height_folder_regex("^(\\d{1,2})cm$");

vector<mat_path_with_azimuth> find_mat_files_in_folder(
        const path& folder_for_height,
        const regex& data_file_regex) {

    auto file_iterator = recursive_directory_iterator{folder_for_height};
    vector<mat_path_with_azimuth> paths;
    for (const auto& dir_entry: file_iterator) {
        smatch sm;
        const path& file_path = dir_entry.path();
        const string filename = file_path.filename().string();
        if (!regex_match(filename, sm, data_file_regex)) {
            continue;
        }
        const azimuth_t azimuth = std::stoi(sm[1]);
        paths.push_back(mat_path_with_azimuth{
                .file_path = file_path,
                .azimuth = azimuth,
        });
    }

    return paths;
}

vector<height_t> parse_available_heights(const path& data_path) {
    vector<height_t> result;
    for (auto const& dir_entry: directory_iterator{data_path}) {
        smatch sm;
        auto path_string = dir_entry.path().filename().string();
        if (!regex_match(path_string, sm, height_folder_regex)) {
            continue;
        }

        height_t height = stoi(sm[1]);
        result.push_back(height);
    }
    return result;
}