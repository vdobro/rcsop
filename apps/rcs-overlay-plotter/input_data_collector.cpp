#include "input_data_collector.h"

#include <regex>

#include "utils/chronometer.h"

using std::filesystem::directory_iterator;
using std::filesystem::recursive_directory_iterator;
using std::regex;
using std::smatch;
using std::shared_ptr;
using std::make_shared;
using std::domain_error;

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

static map<azimuth_t, shared_ptr<az_data>>
map_azimuth_angles_to_data(height_t height, const path& data_path) {

    auto folder_name = std::to_string(height) + "cm";
    const path height_path{data_path / folder_name};
    map<azimuth_t, shared_ptr<az_data>> azimuth_to_data;
    for (auto const& dir_entry: recursive_directory_iterator{height_path}) {
        smatch sm;
        auto file_path = dir_entry.path().string();
        auto filename = dir_entry.path().filename().string();
        if (!regex_match(filename, sm, mat_file_regex)) {
            continue;
        }
        azimuth_t azimuth = stoi(sm[1]);
        data_eval_position position = {
                .height = height,
                .azimuth = azimuth,
        };
        auto data = make_shared<az_data>(file_path, position);
        azimuth_to_data.insert(make_pair(azimuth, data));
    }
    return azimuth_to_data;
}

static map<height_t, map<azimuth_t, shared_ptr<az_data>>>
collect_all_heights(const path& data_path) {
    auto start = start_time();
    auto heights = parse_available_heights(data_path);
    map<height_t, map<azimuth_t, shared_ptr<az_data>>> result;
    for (auto height: heights) {
        map<azimuth_t, shared_ptr<az_data>> angle_mapping = map_azimuth_angles_to_data(height, data_path);
        result.insert(make_pair(height, angle_mapping));
    }
    log_and_start_next(start, "Reading .mat files finished");
    return result;
}

InputDataCollector::InputDataCollector(const path& root_path) {
    this->_root_path = root_path;
    this->_image_path = path{this->_root_path / "images"};

    this->collect_images();
    this->collect_models();
    this->collect_rcs_data();
}

void InputDataCollector::collect_images() {
    for (auto const& dir_entry: directory_iterator{_image_path}) {
        smatch sm;
        const path& file_path = dir_entry.path();
        const path file_name = file_path.filename();
        if (file_name.extension().string() != ".png") {
            continue;
        }
        this->_image_names.push_back(file_path);
    }
    std::sort(this->_image_names.begin(), this->_image_names.end());
}

void InputDataCollector::collect_models() {
    const path models_path{this->_root_path / "models"};
    for (auto const& dir_entry: directory_iterator{models_path}) {
        const path& entry_path = dir_entry.path();
        if (std::filesystem::is_directory(entry_path)) {
            this->_model_paths.at(ModelType::SPARSE_COLMAP).push_back(entry_path);
        }
        if (std::filesystem::is_regular_file(entry_path)) {
            this->_model_paths.at(ModelType::MESH_PLY).push_back(entry_path);
        }
    }
}

void InputDataCollector::collect_rcs_data() {
    const path rcs_data_path{this->_root_path / "data"};
    for (auto const& dir_entry: directory_iterator{rcs_data_path}) {
        const path& entry_path = dir_entry.path();
        if (std::filesystem::is_directory(entry_path)) {
            this->_rcs_data_paths.at(RcsDataType::AZIMUTH_RCS).push_back(entry_path);
        }
        if (std::filesystem::is_regular_file(entry_path)) {
            this->_rcs_data_paths.at(RcsDataType::SIMPLE_RCS_MAT).push_back(entry_path);
        }
    }
}

vector<path> InputDataCollector::image_paths() const {
    return this->_image_names;
}

//TODO caching? eager loading?
map<height_t, map<azimuth_t, shared_ptr<az_data>>> InputDataCollector::rcs_azimuth_data() const {
    vector<path> paths = this->_rcs_data_paths.at(RcsDataType::AZIMUTH_RCS);
    if (paths.empty()) {
        throw domain_error("No azimuth RCS data");
    }
    if (paths.size() > 1) {
        throw domain_error("More than one set of azimuth RCS data available, can not choose");
    }
    const path& path = paths[0];

    return collect_all_heights(path);
}

rcs_data InputDataCollector::simple_rcs_data() const {
    vector<path> paths = this->_rcs_data_paths.at(RcsDataType::SIMPLE_RCS_MAT);
    if (paths.empty()) {
        throw domain_error("No RCS data");
    }
    if (paths.size() > 1) {
        throw domain_error("More than one set of RCS data available, can not choose");
    }
    const path& path = paths[0];
    return rcs_data(path);
}
