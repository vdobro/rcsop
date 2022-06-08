#ifndef RCS_OVERLAY_PLOTTER_INPUT_DATA_COLLECTOR_H
#define RCS_OVERLAY_PLOTTER_INPUT_DATA_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include <filesystem>

#include "sparse_cloud.h"
#include "az_data.h"
#include "rcs_data.h"

using std::string;
using std::vector;
using std::map;
using std::filesystem::path;

typedef long height_t;
typedef long azimuth_t;

class InputDataCollector {
private:
    enum ModelType {
        SPARSE_COLMAP = 0,
        MESH_PLY,
    };

    enum RcsDataType {
        SIMPLE_RCS_MAT = 0,
        AZIMUTH_RCS,
    };

    path _root_path;
    path _image_path;
    vector<path> _image_names;
    map<ModelType, vector<path>> _model_paths = {
            {ModelType::SPARSE_COLMAP, vector<path>{}},
            {ModelType::MESH_PLY,      vector<path>{}}
    };
    map<RcsDataType, vector<path>> _rcs_data_paths = {
            {RcsDataType::SIMPLE_RCS_MAT, vector<path>{}},
            {RcsDataType::AZIMUTH_RCS,    vector<path>{}}
    };

    void collect_images();

    void collect_models();

    void collect_rcs_data();

public:
    explicit InputDataCollector(const path& root_path);

    [[nodiscard]] vector<path> image_paths() const;

    [[nodiscard]] map<height_t, map<azimuth_t, shared_ptr<az_data>>> rcs_azimuth_data() const ;

    [[nodiscard]] rcs_data simple_rcs_data() const;
};


#endif //RCS_OVERLAY_PLOTTER_INPUT_DATA_COLLECTOR_H
