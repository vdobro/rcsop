#ifndef RCSOP_DATA_INPUT_DATA_COLLECTOR_H
#define RCSOP_DATA_INPUT_DATA_COLLECTOR_H

#include "utils/types.h"

#include "sparse_cloud.h"
#include "dense_cloud.h"
#include "basic_rcs_map.h"
#include "azimuth_rcs_data_collection.h"
#include "azimuth_minimap_provider.h"
#include "input_image.h"

namespace rcsop::data {
    using std::domain_error;
    using rcsop::common::camera_options;

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
        explicit InputDataCollector(const path& root_path, const camera_options& options);

        [[nodiscard]] vector<CameraInputImage> images() const;

        template<InputAssetType AssetType>
        shared_ptr<InputAssetDataType<AssetType>> data(bool allow_multiple = false) const {
            const string asset_description = inputAssetTypeDescriptions[AssetType];

            vector<path> asset_paths = _asset_paths.at(AssetType);
            if (asset_paths.empty()) {
                throw domain_error("No data: " + asset_description);
            }
            if (!allow_multiple && asset_paths.size() > 1) {
                throw domain_error("More than one set of data: " + asset_description);
            }
            const path& asset_path = asset_paths[0];
            return make_shared<InputAssetDataType<AssetType>>(asset_path);
        }

        template<InputAssetType AssetType>
        [[nodiscard]] bool data_available() const {
            vector<path> asset_paths = _asset_paths.at(AssetType);
            return !asset_paths.empty();
        }
    };
}

#endif //RCSOP_DATA_INPUT_DATA_COLLECTOR_H
