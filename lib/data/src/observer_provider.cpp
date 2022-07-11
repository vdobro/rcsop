#include "observer_provider.h"

#include "utils/mapping.h"
#include "utils/points.h"

namespace rcsop::data {
    using rcsop::common::utils::points::Vector3d;
    using rcsop::common::utils::map_vec;
    using rcsop::common::camera;

    static vector<camera> filter_with_positions(const vector<camera>& cameras) {
        vector<camera> result;
        std::copy_if(cameras.cbegin(), cameras.cend(),
                     std::back_inserter(result),
                     [](const camera& camera) {
                         const optional<azimuth_t> angle = CameraInputImage::parse_angle_from_name(camera.get_name());
                         return angle.has_value(); // a camera is positioned if at least its angle can be identified by its name
                     });
        return result;
    }

    static double calculate_units_per_centimeter(double camera_distance_to_origin,
                                                 const vector<camera>& cameras) {
        auto positioned_cameras = filter_with_positions(cameras);
        auto observer_positions = map_vec<camera, Vector3d, true>(positioned_cameras, [](const auto& camera) {
            return camera.position();
        });

        auto camera_count = positioned_cameras.size();

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
        const auto result = (average / 2) / camera_distance_to_origin;

        if (result == 0 || std::isnan(result)) {
            throw invalid_argument("World scale must not be zero and not NaN");
        }
        return result;
    }

    ObserverProvider::ObserverProvider(const InputDataCollector& input,
                                       const camera_options& camera_options) {
        if (!input.data_available<SPARSE_CLOUD_COLMAP>()) {
            throw invalid_argument("No COLMAP model");
        }
        auto model = input.data<SPARSE_CLOUD_COLMAP>(false);
        auto cameras = model->get_cameras();

        this->_units_per_centimeter = calculate_units_per_centimeter(
                camera_options.distance_to_origin, cameras);

        auto source_images = input.images();
        vector<Observer> all_observers;

        for (const auto& image: source_images) {
            auto image_path = image.file_path();
            auto image_name = image_path.filename().string();

            for (const auto& camera: cameras) {
                if (image_name == camera.get_name()) {
                    Observer observer(image.position(), image_path, camera, camera_options);
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

        for (auto& observer: _positioned_observers) observer.set_units_per_centimeter(_units_per_centimeter);
        for (auto& observer: _auxiliary_observers) observer.set_units_per_centimeter(_units_per_centimeter);
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
}
