#include "base_point_cloud.h"

#include <utility>

namespace rcsop::common {

    auto BasePointCloud::model_path() -> path {
        return this->_model_path;
    }

    BasePointCloud::BasePointCloud(path model_path) : _model_path(std::move(model_path)) {
    }
}