#include "observer_provider.h"

#include <stdexcept>
#include "utils/mapping.h"

using std::invalid_argument;

static double calculate_units_per_centimeter(double camera_distance_to_origin,
                                             const vector<Observer>& observers_in_a_circle) {
    auto observer_positions = map_vec<Observer, Vector3d, true>(
            observers_in_a_circle, [](const auto& observer) {
                return observer.native_camera().position();
            });

    auto camera_count = observer_positions.size();

    vector<double> distances;
    distances.resize(camera_count);
    for (size_t i = 0; i < camera_count; ++i) {
        auto opposing_index = (i + (camera_count / 2)) % camera_count;

        auto current_camera = observer_positions[i];
        auto opposing_camera = observer_positions[opposing_index];
        auto distance = (current_camera - opposing_camera).norm();
        distances[i] = distance;
    }
    auto average = std::reduce(distances.begin(), distances.end()) / static_cast<double>(camera_count);
    return (average / 2) / camera_distance_to_origin;
}

ObserverProvider::ObserverProvider(const InputDataCollector& input,
                                   double distance_to_origin,
                                   CameraCorrectionParams default_observer_correction) {
    if (!input.data_available<SPARSE_CLOUD_COLMAP>()) {
        throw invalid_argument("No COLMAP model");
    }
    auto model = input.data<SPARSE_CLOUD_COLMAP>(false);
    auto cameras = model->get_cameras();
    auto source_images = input.images();
    vector<Observer> all_observers;

    for (const auto& image: source_images) {
        auto image_path = image.file_path();
        auto image_name = image_path.filename().string();

        for (const auto& camera: cameras) {
            if (image_name == camera.get_name()) {
                Observer observer(image.position(), image_path, camera,
                                  default_observer_correction);
                all_observers.push_back(observer);
                break;
            }
        }
    }
    std::sort(std::execution::par_unseq,
              all_observers.begin(), all_observers.end(), [](const Observer& a, const Observer& b) {
                return a.source_image_path().filename().string()
                       < b.source_image_path().filename().string();
            });
    for (auto& observer: all_observers) {
        observer.set_units_per_centimeter(_units_per_centimeter);
        if (observer.has_position()) {
            this->_positioned_observers.push_back(observer);
        } else {
            this->_auxiliary_observers.push_back(observer);
        }
    }

    this->_units_per_centimeter = calculate_units_per_centimeter(distance_to_origin, _positioned_observers);
    if (_units_per_centimeter == 0) {
        throw invalid_argument("World scale must not be zero");
    }
    for (auto& observer : _positioned_observers) observer.set_units_per_centimeter(_units_per_centimeter);
    for (auto& observer : _auxiliary_observers) observer.set_units_per_centimeter(_units_per_centimeter);
}

vector<Observer> ObserverProvider::observers_with_positions() const {
    return this->_positioned_observers;
}

vector<Observer> ObserverProvider::all_observers() const {
    return this->_auxiliary_observers;
}

double ObserverProvider::get_units_per_centimeter() const {
    return this->_units_per_centimeter;
}
