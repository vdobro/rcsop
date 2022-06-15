#include "observer_provider.h"
#include "point_cloud_provider.h"

ObserverProvider::ObserverProvider(const InputDataCollector& input) {
    if (!input.data_available<SPARSE_CLOUD_COLMAP>()) {
        throw invalid_argument("No COLMAP model");
    }
    auto model = input.data<SPARSE_CLOUD_COLMAP>(false);
    auto cameras = model->get_cameras();
    auto world_scale = PointCloudProvider(input).get_world_scale();
    if (world_scale == 0) {
        throw invalid_argument("World scale must not be zero");
    }

    auto source_images = input.images();

    for (const auto& image: source_images) {
        auto image_path = image.file_path();
        auto image_name = image_path.filename().string();

        for (const auto& camera: cameras) {
            if (image_name == camera.get_name()) {
                Observer observer(image.position(), camera, image_path, world_scale);
                _observers.push_back(observer);
                break;
            }
        }
    }
}

vector<Observer> ObserverProvider::observers() const {
    return this->_observers;
}
