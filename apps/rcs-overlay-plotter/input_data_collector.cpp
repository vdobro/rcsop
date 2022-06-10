#include "input_data_collector.h"

#include <filesystem>

using std::shared_ptr;
using std::make_shared;
using std::filesystem::directory_iterator;

InputDataCollector::InputDataCollector(const path& root_path) {
    this->_root_path = root_path;
    this->_image_path = path{this->_root_path / "images"};

    this->collect_images();
    this->collect_models();
    this->collect_rcs_data();
}

void InputDataCollector::collect_images() {
    for (auto const& dir_entry: directory_iterator{_image_path}) {
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
            this->_asset_paths.at(InputAssetType::SPARSE_CLOUD_COLMAP).push_back(entry_path);
        }
        if (std::filesystem::is_regular_file(entry_path)) {
            this->_asset_paths.at(InputAssetType::DENSE_MESH_PLY).push_back(entry_path);
        }
    }
}

void InputDataCollector::collect_rcs_data() {
    const path rcs_data_path{this->_root_path / "data"};
    for (auto const& dir_entry: directory_iterator{rcs_data_path}) {
        const path& entry_path = dir_entry.path();
        if (std::filesystem::is_directory(entry_path)) {
            this->_asset_paths.at(InputAssetType::AZIMUTH_RCS_MAT).push_back(entry_path);
        }
        if (std::filesystem::is_regular_file(entry_path)) {
            this->_asset_paths.at(InputAssetType::SIMPLE_RCS_MAT).push_back(entry_path);
        }
    }
}

vector<path> InputDataCollector::image_paths() const {
    return this->_image_names;
}
