#include "azimuth_rcs_map.h"

#include <regex>
#include <filesystem>

using std::filesystem::directory_iterator;
using std::filesystem::recursive_directory_iterator;
using std::regex;
using std::smatch;

#include "utils/chronometer.h"

static const regex mat_file_regex("^DataAuswertung_\\d{1,2}cm_(\\d{1,3})°\\.mat$");
static const regex height_folder_regex("^(\\d{1,2})cm$");

static vector<height_t> parse_available_heights(const path& data_path) {
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

static map<azimuth_t, shared_ptr<AzimuthRcsDataSet>>
map_azimuth_angles_to_data(height_t height, const path& data_path) {

    auto folder_name = std::to_string(height) + "cm";
    const path height_path{data_path / folder_name};
    map<azimuth_t, shared_ptr<AzimuthRcsDataSet>> azimuth_to_data;
    for (auto const& dir_entry: recursive_directory_iterator{height_path}) {
        smatch sm;
        const path& directory_path = dir_entry.path();
        const string file_path = directory_path.string();
        const string filename = directory_path.filename().string();
        if (!regex_match(filename, sm, mat_file_regex)) {
            continue;
        }
        azimuth_t azimuth = stoi(sm[1]);
        ObserverPosition position = {
                .height = height,
                .azimuth = azimuth,
        };
        auto data = make_shared<AzimuthRcsDataSet>(file_path, position);
        azimuth_to_data.insert(make_pair(azimuth, data));
    }
    return azimuth_to_data;
}

static void collect_all_heights(const path& data_path,
                                shared_ptr<azimuth_map_t> result) {
    auto start = start_time();
    auto heights = parse_available_heights(data_path);
    result->clear();
    for (auto height: heights) {
        map<azimuth_t, shared_ptr<AzimuthRcsDataSet>> angle_mapping = map_azimuth_angles_to_data(height, data_path);
        result->insert(make_pair(height, angle_mapping));
    }
    log_and_start_next(start, "Reading .mat files finished");
}

AzimuthRcsMap::AzimuthRcsMap(const path& input_path) {
    this->data = std::make_shared<azimuth_map_t>();
    collect_all_heights(input_path, this->data);
}

map<azimuth_t, shared_ptr<AzimuthRcsDataSet>> AzimuthRcsMap::at_height(height_t height) const {
    return this->data->at(height);
}

shared_ptr<AzimuthRcsDataSet> AzimuthRcsMap::at_position(const ObserverPosition& position) const {
    return this->at_height(position.height).at(position.azimuth);
}

vector<height_t> AzimuthRcsMap::heights() const {
    vector<height_t > result;
    for (auto const& pair : *this->data) {
        result.push_back(pair.first);
    }
    return result;
}
