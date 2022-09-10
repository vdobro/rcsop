#include "input_data_collector.h"

namespace rcsop::data {
    using std::filesystem::directory_iterator;
    using rcsop::common::utils::sort_in_place;

    InputDataCollector::InputDataCollector(const path& root_path, const camera_options& options) {
        this->_root_path = root_path;
        this->_image_path = path{this->_root_path / "images"};

        this->collect_images(options);
        this->collect_models();
        this->collect_rcs_data();
    }

    void InputDataCollector::collect_images(const camera_options& options) {
        if (!exists(_image_path)) {
            std::clog << "WARNING: no folder 'images' found, images will not be collected" << std::endl;
            return;
        }
        for (auto const& image_dir_entry: recursive_directory_iterator{_image_path}) {
            const path& file_path = image_dir_entry.path();
            const path file_name = file_path.filename();
            if (file_name.extension().string() != ".png") {
                continue;
            }
            this->_images.emplace_back(file_path, _image_path, options.default_height);
        }
        sort_in_place<CameraInputImage, string>(this->_images, [](const CameraInputImage& a) {
            return a.image_name();
        });
    }

    void InputDataCollector::collect_models() {
        const path models_path{this->_root_path / "models"};
        if (!exists(models_path)) {
            std::clog << "WARNING: no folder 'models' found, point cloud data will not be imported" << std::endl;
            return;
        }
        for (auto const& dir_entry: directory_iterator{models_path}) {
            const path& entry_path = dir_entry.path();
            if (is_directory(entry_path)) {
                this->_asset_paths.at(InputAssetType::SPARSE_CLOUD_COLMAP).push_back(entry_path);
            }
            if (is_regular_file(entry_path) && entry_path.extension().string() == ".ply") {
                this->_asset_paths.at(InputAssetType::DENSE_MESH_PLY).push_back(entry_path);
            }
        }
    }

    void InputDataCollector::collect_rcs_data() {
        const path rcs_data_path{this->_root_path / "data"};
        if (!exists(rcs_data_path)) {
            std::clog << "WARNING: no folder 'data' found, RCS data will not be imported" << std::endl;
            return;
        }
        for (auto const& dir_entry: directory_iterator{rcs_data_path}) {
            const path& entry_path = dir_entry.path();
            if (is_directory(entry_path)) {
                this->_asset_paths.at(InputAssetType::AZIMUTH_RCS_MAT).push_back(entry_path);
                this->_asset_paths.at(InputAssetType::AZIMUTH_RCS_MINIMAP).push_back(entry_path);
            }
            if (is_regular_file(entry_path)) {
                this->_asset_paths.at(InputAssetType::SIMPLE_RCS_MAT).push_back(entry_path);
            }
        }
    }

    auto InputDataCollector::images() const -> vector<CameraInputImage> {
        return this->_images;
    }
}
