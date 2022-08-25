#include "observer_provider.h"

#include "utils/mapping.h"
#include "utils/points.h"

#include "colmap_observer_camera.h"

using rcsop::common::ColmapObserverCamera;

namespace rcsop::data {
    using rcsop::common::utils::points::vec3;
    using rcsop::common::utils::map_vec;
    using rcsop::common::ModelCamera;
    using rcsop::common::height_t;

    static map<height_t, vector<ModelCamera>> filter_with_positions(
            const camera_options options,
            const vector<ModelCamera>& cameras) {
        map<height_t, vector<ModelCamera>> result;
        for (const auto& camera: cameras) {
            auto full_name = camera.get_name();
            std::stringstream name_stream(full_name);
            vector<string> segment_list;
            string segment;
            while (std::getline(name_stream, segment, std::filesystem::path::preferred_separator)) {
                segment_list.push_back(segment);
            }
            auto last_segment = segment_list.at(segment_list.size() - 1);
            const optional<azimuth_t> angle = CameraInputImage::parse_angle_from_name(last_segment);
            if (!angle.has_value()) continue; // a camera is positioned if at least its angle can be identified by its name

            auto position = CameraInputImage::parse_position_from_name(last_segment);
            auto height = position.has_value() ? position->height : options.default_height;

            if (!result.contains(height)) {
                result[height] = vector<ModelCamera>{};
            }
            result[height].push_back(camera);
        }
        return result;
    }

    static vec3 get_farthest_camera_position(const vec3& camera_position,
                                             const vector<vec3>& positions) {
        auto iterator = std::max_element(
                positions.cbegin(), positions.cend(),
                [&camera_position](const vec3& a, const vec3& b) {
                    return (camera_position - a).norm() < (camera_position - b).norm();
                });
        auto index = std::distance(positions.cbegin(), iterator);
        return positions[index];
    }

    static double average_distance_to_center(const vector<ModelCamera>& cameras) {
        auto observer_positions = map_vec<ModelCamera, vec3, true>(cameras, [](const auto& camera) {
            return camera.position();
        });

        auto camera_count = cameras.size();

        vector<double> distances = map_vec<vec3, double>(
                observer_positions,
                [&observer_positions](const vec3& position) -> double {
                    auto farthest_camera = get_farthest_camera_position(position, observer_positions);
                    return (position - farthest_camera).norm();
                });
        auto average = std::reduce(distances.begin(), distances.end()) / static_cast<double>(camera_count);
        auto half_average = average / 2;
        return half_average;
    }

    static double calculate_units_per_centimeter(const camera_options options,
                                                 const vector<ModelCamera>& cameras) {
        auto positioned_cameras = filter_with_positions(options, cameras);
        vector<double> averages_per_height;
        for (const auto& [height, cameras_at_height]: positioned_cameras) {
            double height_average = average_distance_to_center(cameras_at_height);
            averages_per_height.push_back(height_average);
        }
        auto total_average = std::reduce(averages_per_height.begin(), averages_per_height.end())
                             / static_cast<double>(averages_per_height.size());

        const auto result = total_average / options.distance_to_origin;

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
        auto model = input.data<SPARSE_CLOUD_COLMAP>();
        auto cameras = model->get_cameras();

        this->_units_per_centimeter = calculate_units_per_centimeter(camera_options, cameras);

        auto source_images = input.images();
        vector<Observer> all_observers;

        for (const auto& image: source_images) {
            const auto image_name = image.image_name();

            for (const auto& camera: cameras) {
                if (!(image_name == camera.get_name())) {
                    continue;
                }

                auto observer_camera = make_shared<ColmapObserverCamera>(camera, camera_options.pitch_correction);
                all_observers.emplace_back(image.position(), image.file_path(), observer_camera);
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

    auto ObserverProvider::observers_with_positions() const -> vector<Observer> {
        return this->_positioned_observers;
    }

    auto ObserverProvider::all_observers() const -> vector<Observer> {
        return this->_auxiliary_observers;
    }

    auto ObserverProvider::get_units_per_centimeter() const -> double {
        return this->_units_per_centimeter;
    }
}
