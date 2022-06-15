#include "azimuth_angles.h"

#include <regex>
#include <filesystem>

#include "az_data.h"
#include "relative_points.h"
#include "render_points.h"

#include "utils/chronometer.h"
#include "options.h"

using std::make_pair;
using std::make_shared;
using std::smatch;
using std::regex_match;
using std::stoi;
using std::invalid_argument;
using std::filesystem::directory_iterator;
using std::filesystem::recursive_directory_iterator;

using std::regex;
static const regex mat_file_regex("^DataAuswertung_\\d{1,2}cm_(\\d{1,3})°\\.mat$");
static const regex image_name_regex("^(\\d{3})°.*\\.png$");
static const regex height_folder_regex("^(\\d{1,2})cm$");

typedef long height_t;
typedef long azimuth_t;

using namespace sfm::rendering;

static long get_camera_azimuth(const camera& image) {
    smatch sm;
    string image_name = image.get_name();
    if (!regex_match(image_name, sm, image_name_regex)) {
        throw invalid_argument("image");
    }
    return stoi(sm[1]);
}

static vector<height_t> parse_available_heights(const path& data_path) {
    vector<height_t> result;
    for (auto const& dir_entry: directory_iterator{data_path}) {
        smatch sm;
        auto path_string = dir_entry.path().filename().string();
        if (!regex_match(path_string, sm, height_folder_regex)) {
            continue;
        }

        height_t height = stoi(sm[1]);
        result.push_back(height);
    }
    return result;
}

static map<azimuth_t, shared_ptr<az_data>> map_azimuth_angles_to_data(height_t height, const path& data_path) {
    auto folder_name = std::to_string(height) + "cm";
    const path height_path{data_path / folder_name};
    map<azimuth_t, shared_ptr<az_data>> azimuth_to_data;
    for (auto const& dir_entry: recursive_directory_iterator{height_path}) {
        smatch sm;
        auto file_path = dir_entry.path().string();
        auto filename = dir_entry.path().filename().string();
        if (!regex_match(filename, sm, mat_file_regex)) {
            continue;
        }
        azimuth_t azimuth = stoi(sm[1]);
        ObserverPosition position = {
                .height = height,
                .azimuth = azimuth,
        };
        auto data = make_shared<az_data>(file_path, position);
        azimuth_to_data.insert(make_pair(azimuth, data));
    }
    return azimuth_to_data;
}

static map<height_t, map<azimuth_t, shared_ptr<az_data>>> collect_all_heights(const vector<height_t>& heights,
                                                                              const path& data_path) {
    auto start = start_time();
    map<height_t, map<azimuth_t, shared_ptr<az_data>>> result;
    for (auto height: heights) {
        map<azimuth_t, shared_ptr<az_data>> angle_mapping = map_azimuth_angles_to_data(height, data_path);
        result.insert(make_pair(height, angle_mapping));
    }
    log_and_start_next(start, "Reading .mat files finished");
    return result;
}

static map<camera_id_t, map<height_t, shared_ptr<az_data>>> map_cameras_to_azimuth_data(
        const vector<camera>& cameras,
        const path& data_path,
        const vector<height_t>& heights) {
    auto height_to_azimuth_to_data = collect_all_heights(heights, data_path);

    map<camera_id_t, map<height_t, shared_ptr<az_data>>> camera_to_height_to_data;
    for (const camera& camera: cameras) {
        azimuth_t azimuth = get_camera_azimuth(camera);
        map<height_t, shared_ptr<az_data>> height_to_data;
        for (auto height: heights) {
            auto data = height_to_azimuth_to_data.at(height).at(azimuth);
            height_to_data.insert(make_pair(height, data));
        }
        camera_to_height_to_data.insert(make_pair(camera.id(), height_to_data));
    }
    return camera_to_height_to_data;
}

shared_ptr<point_display_payload> display_azimuth(const shared_ptr<SparseCloud>& model,
                                                  const shared_ptr<DenseCloud>& dense_model,
                                                  const path& image_path,
                                                  const path& data_path,
                                                  const path& output_path) {
    auto cameras = model->get_cameras();
    auto camera_count = cameras.size();
    vector<long> heights = parse_available_heights(data_path);
    heights.clear(); //TODO WIP
    heights.push_back(40); //TODO height data should not be summed at all
    auto data_by_camera = map_cameras_to_azimuth_data(cameras, data_path, heights);
    auto world_scale = model->get_world_scale(CAMERA_DISTANCE);
    map<point_id_t, scored_point> scored_points = model->get_scored_points();
    auto dense_points = dense_model->points();
    point_id_t dense_point_id = 500000;
    for (const auto& dense_point: dense_points) {
        auto point_id = dense_point_id++;
        scored_points.insert(make_pair(point_id, scored_point(dense_point, point_id, 0)));
    }

    auto render_path = path{output_path / "render"};
    std::filesystem::remove_all(render_path);
    create_directories(render_path);

    auto time_measure = start_time();
    vector<scored_point> all_points; // ONLY use for colormap calibration
    map<camera_id_t, vector<scored_point>> camera_to_points;
    for (const camera& camera: cameras) {
        auto log_prefix = get_log_prefix(camera.id(), camera_count);
        scored_point_map camera_points(scored_points);
        map<height_t, shared_ptr<az_data>> height_to_data = data_by_camera.at(camera.id());
        for (auto& height: heights) {
            shared_ptr<az_data> data = height_to_data.at(height);
            auto height_offset = static_cast<double>(height - DEFAULT_HEIGHT) * world_scale;
            auto relative_points = get_point_angles(camera, height_offset, camera_points);
            for (const auto& point: relative_points) {
                if (point.horizontal_angle > 5) {
                //if (point.distance_to_horizontal_plane > fabs(height_offset / 2.0)) {
                    continue;
                }
                auto point_distance = point.distance / world_scale;
                auto value = data->find_nearest(point_distance, point.horizontal_angle);
                if (!std::isnan(value)) {
                    camera_points.at(point.id).increment_score(value);
                }
            }
        }
        time_measure = log_and_start_next(time_measure,
                                          "Scoring of " + std::to_string(scored_points.size()) + " points for "
                                          + std::to_string(camera_count) + " images");
        vector<scored_point> filtered_points;
        for (const auto& point: camera_points) {
            auto old_point = point.second;
            if (old_point.score_to_dB() < -20 || old_point.score_to_dB() > 5) {
                continue;
            }
            filtered_points.push_back(old_point);
            all_points.push_back(old_point);
        }
        time_measure = log_and_start_next(time_measure,
                                          log_prefix + "\tFiltered " + std::to_string(filtered_points.size())
                                          + " from a total of " + std::to_string(scored_points.size())
                                          + " points");
        camera_to_points.insert(make_pair(camera.id(), filtered_points));
    }
    auto scores = map_vec<scored_point, double>(all_points, [](const scored_point& point) {
        return point.score_to_dB();
    });

    auto min_score = *std::min_element(scores.begin(), scores.end());
    auto max_score = *std::max_element(scores.begin(), scores.end());
    return make_shared<point_display_payload>(point_display_payload{
            .min_value = min_score,
            .max_value = max_score,
            .points = camera_to_points,
            .image_path = image_path,
    });
}

void render_to_files(const point_display_payload& point_payload,
                     const path& output_path) {
    const auto colormap = construct_colormap_function(COLOR_MAP, point_payload.min_value, point_payload.max_value);
    auto camera_count = cameras.size();
    auto image_path = point_payload.image_path;

    auto shader = initialize_renderer();
    for (const camera& camera: cameras) {
        auto log_prefix = get_log_prefix(camera.id(), camera_count);
        auto points = point_payload.points.at(camera.id());
        render_image(camera, shader,
                     image_path, output_path,
                     points, colormap, log_prefix);
    }
}
