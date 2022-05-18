#include "azimuth_angles.h"

using std::make_pair;
using std::make_shared;
using std::smatch;
using std::regex_match;
using std::stoi;
using std::invalid_argument;
using std::filesystem::directory_iterator;
using std::filesystem::recursive_directory_iterator;

using colmap::image_t;

using std::regex;
static const regex mat_file_regex("^DataAuswertung_\\d{1,2}cm_(\\d{1,3})°\\.mat$");
static const regex image_name_regex("^(\\d{3})°.*\\.png$");
static const regex height_folder_regex("^(\\d{1,2})cm$");

typedef long height_t;
typedef long azimuth_t;

static long get_image_azimuth(const Image& image) {
    smatch sm;
    if (!regex_match(image.Name(), sm, image_name_regex)) {
        throw invalid_argument("image");
    }
    return stoi(sm[1]);
}

static vector<height_t> get_heights(const path& data_path) {
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

static map<azimuth_t, shared_ptr<az_data>> get_azimuth_to_data_mapping(height_t height, const path& data_path) {
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
        data_eval_position position = {
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
        map<azimuth_t, shared_ptr<az_data>> angle_mapping = get_azimuth_to_data_mapping(height, data_path);
        result.insert(make_pair(height, angle_mapping));
    }
    log_and_start_next(start, "Reading .mat files finished");
    return result;
}

static map<image_t, map<height_t, shared_ptr<az_data>>> get_image_data_map(
        const vector<Image>& images,
        const path& data_path,
        const vector<height_t>& heights) {
    auto height_to_azimuth_to_data = collect_all_heights(heights, data_path);

    map<image_t, map<height_t, shared_ptr<az_data>>> image_to_height_to_data;
    for (const auto& image: images) {
        azimuth_t azimuth = get_image_azimuth(image);
        map<height_t, shared_ptr<az_data>> height_to_data;
        for (auto height: heights) {
            auto data = height_to_azimuth_to_data.at(height).at(azimuth);
            height_to_data.insert(make_pair(height, data));
        }
        image_to_height_to_data.insert(make_pair(image.ImageId(), height_to_data));
    }
    return image_to_height_to_data;
}

void display_azimuth(const model_ptr& model,
                     const path& image_path,
                     const path& data_path,
                     const path& output_path) {
    auto images = get_images(*model);

    //TODO: clarify calibration/normalization across multiple files
    auto heights = get_heights(data_path);

    auto data_by_image = get_image_data_map(images, data_path, heights);
    auto world_scale = get_world_scale(CAMERA_DISTANCE, *model);
    auto scored_points = get_scored_points(*model);

    auto render_path = path{output_path / "render"};
    std::filesystem::remove_all(render_path);
    create_directories(render_path);

    auto shader = initialize_renderer();

    auto time_measure = start_time();
    vector<scored_point> all_points; // ONLY for colormap calibration, TODO check if needed
    map<image_t, vector<scored_point>> image_to_points;
    for (const auto& image: images) {
        auto log_prefix = get_log_prefix(image.ImageId(), images.size());
        map<height_t, shared_ptr<az_data>> height_to_data = data_by_image.at(image.ImageId());
        for (auto& height: heights) {
            shared_ptr<az_data> data = height_to_data.at(height);
            auto height_offset = static_cast<double>(height - DEFAULT_HEIGHT) * world_scale;
            auto relative_points = get_point_angles(image, height_offset, scored_points);
            for (const auto& point: relative_points) {
                if (point.horizontal_angle > 5) {
                    //TODO fix for more than +-one additional height
                //if (point.distance_to_horizontal_plane > fabs(height_offset / 2.0)) {
                    continue;
                }
                auto point_distance = point.distance / world_scale;
                auto value = data->find_nearest(point_distance, point.horizontal_angle);
                if (!std::isnan(value)) {
                    scored_points.at(point.id).increment_score(value);
                }
            }
        }
        time_measure = log_and_start_next(time_measure,
                                          "Scoring of " + std::to_string(scored_points.size()) + " points for "
                                          + std::to_string(images.size()) + " images");
        vector<scored_point> filtered_points;
        for (const auto& point: scored_points) {
            auto old_point = point.second;
            if (old_point.score() <= 1E-20) {
                //TODO WHY DOES THIS SOMETIMES GET TO ~1E-309 DESPITE ASSIGNED VALUE ?!?!?!
                continue;
            }
            filtered_points.push_back(old_point);
            all_points.push_back(old_point);
        }
        time_measure = log_and_start_next(time_measure,
                                          log_prefix + "\tFiltered " + std::to_string(filtered_points.size())
                                          + " from a total of " + std::to_string(scored_points.size())
                                          + " points");
        image_to_points.insert(make_pair(image.ImageId(), filtered_points));

#ifdef COLORMAP_PER_IMAGE
        const auto colormap = get_colormap(filtered_points, COLOR_MAP);
        render_image(model, image, shader,
                     image_path.string(), render_path.string(),
                     filtered_points, colormap, log_prefix);
#endif
    }

#ifndef COLORMAP_PER_IMAGE
    const auto colormap = get_colormap(all_points, COLOR_MAP);
    for(const auto& image : images) {
        auto log_prefix = get_log_prefix(image.ImageId(), images.size());
        auto points = image_to_points.at(image.ImageId());
        render_image(model, image, shader,
                     image_path.string(), render_path.string(),
                     points, colormap, log_prefix);
    }
#endif

    log_and_start_next(time_measure, "(Total) rendering done.");
}