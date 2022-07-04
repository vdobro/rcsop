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
    vector<Observer> all_observers;

    for (const auto& image: source_images) {
        auto image_path = image.file_path();
        auto image_name = image_path.filename().string();

        for (const auto& camera: cameras) {
            if (image_name == camera.get_name()) {
                Observer observer(image.position(), image_path, camera, world_scale, default_observer_correction);
                all_observers.push_back(observer);
                break;
            }
        }
    }
    for (auto& observer : all_observers) {
        if (observer.has_position()) {
            this->_positioned_observers.push_back(observer);
        } else {
            this->_auxiliary_observers.push_back(observer);
        }
    }
}

vector<Observer> ObserverProvider::observers_with_positions() const {
    return this->_positioned_observers;
}

vector<Observer> ObserverProvider::all_observers() const {
    return this->_auxiliary_observers;
}
