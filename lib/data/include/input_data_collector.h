#ifndef RCSOP_DATA_INPUT_DATA_COLLECTOR_H
#define RCSOP_DATA_INPUT_DATA_COLLECTOR_H

#include "utils/types.h"

#include "sparse_cloud.h"
#include "dense_cloud.h"
#include "basic_rcs_map.h"
#include "azimuth_rcs_data_collection.h"
#include "azimuth_minimap_provider.h"
#include "input_image.h"
#include "model_writer.h"

namespace rcsop::data {
    using std::domain_error;
    using rcsop::data::ModelWriter;
    using rcsop::common::camera_options;
    using rcsop::common::utils::map_vec_shared;

    enum InputAssetType {
        SPARSE_CLOUD_COLMAP = 0,
        DENSE_MESH_PLY = 1,
        SIMPLE_RCS_MAT = 2,
        AZIMUTH_RCS_MAT = 3,
        AZIMUTH_RCS_MINIMAP = 4,
    };

    static const char* inputAssetTypeDescriptions[5] = {
            "Sparse cloud (COLMAP)",
            "Dense mesh (.ply)",
            "RCS sums (rcs.mat)",
            "Azimuth RCS values",
            "Azimuth RCS preview minimaps",
    };

    template<InputAssetType T>
    struct InputAssetTrait {
    };

    template<>
    struct InputAssetTrait<SPARSE_CLOUD_COLMAP> {
        using type = rcsop::common::SparseCloud;
    };

    template<>
    struct InputAssetTrait<DENSE_MESH_PLY> {
        using type = rcsop::common::DenseCloud;
    };

    template<>
    struct InputAssetTrait<SIMPLE_RCS_MAT> {
        using type = rcsop::data::BasicRcsMap;
    };

    template<>
    struct InputAssetTrait<AZIMUTH_RCS_MAT> {
        using type = rcsop::data::AzimuthRcsDataCollection;
    };

    template<>
    struct InputAssetTrait<AZIMUTH_RCS_MINIMAP> {
        using type = rcsop::data::AzimuthMinimapProvider;
    };

    template<InputAssetType T>
    using InputAssetDataType = typename InputAssetTrait<T>::type;

    class InputDataCollector {
    private:
        path _root_path;
        path _image_path;
        vector<CameraInputImage> _images;
        map<InputAssetType, vector<path>> _asset_paths = {
                {InputAssetType::SPARSE_CLOUD_COLMAP, vector<path>{}},
                {InputAssetType::DENSE_MESH_PLY,      vector<path>{}},
                {InputAssetType::SIMPLE_RCS_MAT,      vector<path>{}},
                {InputAssetType::AZIMUTH_RCS_MAT,     vector<path>{}},
                {InputAssetType::AZIMUTH_RCS_MINIMAP, vector<path>{}},
        };

        void collect_images(const camera_options& options);

        void collect_models();

        void collect_rcs_data();

    public:
        InputDataCollector(const path& root_path, const camera_options& options);

        [[nodiscard]] vector<CameraInputImage> images() const;

        template<InputAssetType AssetType, bool Multiple = false>
        [[nodiscard]] auto data() const {
            using ReturnAssetType = InputAssetDataType<AssetType>;
            const string asset_description = inputAssetTypeDescriptions[AssetType];

            vector<path> asset_paths = _asset_paths.at(AssetType);
            if (asset_paths.empty()) {
                throw domain_error("No data: " + asset_description);
            }
            if constexpr (Multiple) {
                map<string, shared_ptr<ReturnAssetType>> labeled_assets;
                for (const auto& asset_path : asset_paths) {
                    string path_suffix = asset_path.filename().string();
                    labeled_assets.insert(make_pair(path_suffix, make_shared<ReturnAssetType>(asset_path)));
                }
                return labeled_assets;
            } else {
                vector<shared_ptr<ReturnAssetType>> result;
                for (const auto& path : asset_paths) {
                    if (ReturnAssetType::is_available_at(path)) {
                        result.push_back(make_shared<ReturnAssetType>(path));
                    }
                }
                if (result.size() > 1) {
                    throw domain_error("More than one set of data: " + asset_description);
                }
                if (result.empty()) {
                    throw domain_error("No set of data available: " + asset_description);
                }
                return result[0];
            }
        }

        template<InputAssetType AssetType>
        [[nodiscard]] bool data_available() const {
            vector<path> asset_paths = _asset_paths.at(AssetType);
            return !asset_paths.empty();
        }

        [[nodiscard]] shared_ptr<ModelWriter> get_model_writer() const {
            if (!data_available<SPARSE_CLOUD_COLMAP>()) {
                throw domain_error("No sparse cloud model available.");
            }
            return make_shared<ModelWriter>(data<SPARSE_CLOUD_COLMAP, false>());
        }
    };
}

#endif //RCSOP_DATA_INPUT_DATA_COLLECTOR_H
