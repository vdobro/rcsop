#include "utils/rcs_data_utils.h"

using std::filesystem::directory_iterator;

static const regex height_folder_regex("^(\\d{1,2})cm$");

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