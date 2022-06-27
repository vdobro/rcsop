#include "observer_provider.h"

#include <stdexcept>
using std::invalid_argument;

#include "point_cloud_provider.h"

ObserverProvider::ObserverProvider(const InputDataCollector& input,
                                   double distance_to_origin,
                                   CameraCorrectionParams default_observer_correction) {
    if (!input.data_available<SPARSE_CLOUD_COLMAP>()) {
        throw invalid_argument("No COLMAP model");
    }
    auto model = input.data<SPARSE_CLOUD_COLMAP>(false);
    auto cameras = model->get_cameras();
    auto world_scale = PointCloudProvider(input).get_world_scale(distance_to_origin);
    if (world_scale == 0) {
        throw invalid_argument("World scale must not be zero");
    }

    auto source_images = input.images();

    for (const auto& image: source_images) {
        auto image_path = image.file_path();
        auto image_name = image_path.filename().string();

        for (const auto& camera: cameras) {
            if (image_name == camera.get_name()) {
                _observers.emplace_back(image.position(), camera, image_path, world_scale, default_observer_correction);
                break;
            }
        }
    }
}

vector<Observer> ObserverProvider::observers() const {
    return this->_observers;
}
