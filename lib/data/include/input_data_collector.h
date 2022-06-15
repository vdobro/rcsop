#ifndef RCSOP_DATA_INPUT_DATA_COLLECTOR_H
#define RCSOP_DATA_INPUT_DATA_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include <filesystem>

#include "sparse_cloud.h"
#include "dense_cloud.h"
#include "rcs_data.h"
#include "azimuth_rcs_map.h"
#include "input_image.h"

using std::string;
using std::vector;
using std::map;
using std::domain_error;
using std::filesystem::path;

enum InputAssetType {
    SPARSE_CLOUD_COLMAP = 0,
    DENSE_MESH_PLY = 1,
    SIMPLE_RCS_MAT = 2,
    AZIMUTH_RCS_MAT = 3,
};

static const char* inputAssetTypeDescriptions[4] = {
        "Sparse cloud (COLMAP)",
        "Dense mesh (.ply)",
        "RCS sums (rcs.mat)",
        "Azimuth RCS values",
};

template<InputAssetType T>
struct InputAssetTrait {
};

template<>
struct InputAssetTrait<SPARSE_CLOUD_COLMAP> {
    using type = SparseCloud;
};

template<>
struct InputAssetTrait<DENSE_MESH_PLY> {
    using type = DenseCloud;
};

template<>
struct InputAssetTrait<SIMPLE_RCS_MAT> {
    using type = rcs_data;
};

template<>
struct InputAssetTrait<AZIMUTH_RCS_MAT> {
    using type = AzimuthRcsMap;
};

template<InputAssetType T>
using InputAssetDataType = typename InputAssetTrait<T>::type;

class InputDataCollector {
private:
    path _root_path;
    path _image_path;
    vector<InputImage> _images;
    map<InputAssetType, vector<path>> _asset_paths = {
            {InputAssetType::SPARSE_CLOUD_COLMAP, vector<path>{}},
            {InputAssetType::DENSE_MESH_PLY,      vector<path>{}},
            {InputAssetType::SIMPLE_RCS_MAT,      vector<path>{}},
            {InputAssetType::AZIMUTH_RCS_MAT,     vector<path>{}},
    };

    void collect_images();

    void collect_models();

    void collect_rcs_data();

public:
    explicit InputDataCollector(const path& root_path);

    [[nodiscard]] vector<InputImage> images() const;

    template<InputAssetType AssetType>
    shared_ptr<InputAssetDataType<AssetType>> data(bool allow_multiple) const {
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

#endif //RCSOP_DATA_INPUT_DATA_COLLECTOR_H
