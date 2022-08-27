#include "observer_provider.h"

#include "utils/mapping.h"
#include "utils/points.h"

#include "colmap_observer_camera.h"
#include "observer_position.h"

using rcsop::common::ColmapObserverCamera;

namespace rcsop::data {
    using rcsop::common::utils::points::vec3;
    using rcsop::common::utils::map_vec;
    using rcsop::common::ModelCamera;
    using rcsop::common::height_t;
    using rcsop::common::ObserverPosition;
    using rcsop::common::observer_camera_options;

    static inline auto filter_with_positions(
            const camera_options options,
            const vector<ModelCamera>& cameras) {
        map<height_t, vector<ModelCamera>> result;
        for (const auto& camera: cameras) {
            auto camera_name = camera.get_last_name_segment();
            auto angle = CameraInputImage::parse_angle_from_name(camera_name);

            // a camera is positioned if at least its angle can be identified by its name
            if (!angle.has_value()) {
                continue;
            }

            auto position = CameraInputImage::parse_position_from_name(camera_name);
            auto height = position.has_value() ? position->height : options.default_height;

            if (!result.contains(height)) {
                result[height] = vector<ModelCamera>{};
            }
            result[height].push_back(camera);
        }
        return result;
    }

    static inline auto get_farthest_camera_position(const vec3& camera_position,
                                                    const vector<vec3>& positions) -> vec3 {
        return rcsop::common::utils::points::find_farthest(
                camera_position, positions);
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

    static void sort_by_image_name(vector<Observer>& observers) {
        std::sort(PARALLEL_EXECUTOR,
                  observers.begin(), observers.end(), [](const Observer& a, const Observer& b) {
                    return a.source_image_path().filename().string()
                           < b.source_image_path().filename().string();
                });
    }

    static auto collect_from_cameras(const vector<CameraInputImage>& source_images,
                                     const vector<ModelCamera>& cameras,
                                     const camera_options& camera_options) -> vector<Observer> {
        vector<Observer> result;
        for (const auto& image: source_images) {
            const auto image_name = image.image_name();

            for (const auto& camera: cameras) {
                if (!(image_name == camera.get_name())) {
                    continue;
                }

                auto observer_camera = make_shared<ColmapObserverCamera>(camera, camera_options.pitch_correction);
                result.emplace_back(image.position(), image.file_path(), observer_camera);
            }
        }
        return result;
    }

    static auto find_nearest_observer(const ObserverPosition position,
                                      const vector<Observer>& positioned_observers,
                                      const camera_options& options) -> optional<Observer> {
        if (options.use_any_camera_nearby) {
            vector<double> distances = map_vec<Observer, double>(
                    positioned_observers,
                    [&position, &options](const Observer& observer) -> double {
                        return position.distance_to(observer.position(), options.distance_to_origin);
                    });
            auto iterator = std::min_element(distances.cbegin(), distances.cend());
            auto index = std::distance(distances.cbegin(), iterator);
            auto result = positioned_observers[index];

            std::clog << "Warning: Observer at " << result.position().str()
                      << " will be used for data set at " << position.str()
                      << std::endl;
            return result;
        } else {
            for (const auto& observer: positioned_observers) {
                if (observer.position().azimuth != position.azimuth) {
                    continue;
                }
                std::clog << "Info: Observer at " << observer.position().str()
                          << " will be used for data set at " << position.str()
                          << std::endl;
                return observer;
            }
            std::clog << "Warning: No viable observer found for data at " << position.str()
                      << ", data set will be skipped entirely." << std::endl;
            return {};
        }
    }

    static auto get_available_positions_from_data(const InputDataCollector& input) -> vector<ObserverPosition> {
        vector<ObserverPosition> result;
        if (!input.data_available<AZIMUTH_RCS_MAT>()) {
            return result;
        }
        auto rcs_data_collections = input.data<AZIMUTH_RCS_MAT, true>();
        set<ObserverPosition> position_set;
        for (const auto& [label, rcs_data]: rcs_data_collections) {
            auto data_positions = rcs_data->available_positions();
            for (auto position: data_positions) {
                position_set.insert(position);
            }
        }
        std::copy(position_set.begin(), position_set.end(), std::back_inserter(result));
        return result;
    }

    static auto collect_missing_data_observers(const vector<Observer>& observers,
                                               const InputDataCollector& input,
                                               const camera_options& options) -> vector<Observer> {
        vector<Observer> result;
        auto positions_from_data = get_available_positions_from_data(input);
        auto existing_positions = Observer::map_to_positions(observers);
        auto positioned_observers = Observer::filter_with_positions(observers);

        for (auto position: positions_from_data) {
            if (existing_positions.contains(position)) {
                continue;
            }
            auto nearest_observer = find_nearest_observer(position, positioned_observers, options);
            if (!nearest_observer.has_value()) {
                continue;
            }
            auto height_difference = position.height - nearest_observer->position().height;
            observer_camera_options translation{
                    .height_offset = height_difference,
            };
            auto observer_with_position = nearest_observer->clone_with_position(position);
            auto observer_with_translation = observer_with_position.clone_with_camera(translation);
            result.push_back(observer_with_translation);
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
        auto all_observers = collect_from_cameras(source_images, cameras, camera_options);
        auto extended_observers = collect_missing_data_observers(all_observers, input, camera_options);
        std::copy(extended_observers.begin(), extended_observers.end(), std::back_inserter(all_observers));
        sort_by_image_name(all_observers);

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
