#include "model.h"

using std::make_shared;
using std::make_pair;
using std::filesystem::create_directories;
using std::filesystem::remove_all;

shared_ptr<Reconstruction> read_model(const path& path) {
    auto model = make_shared<Reconstruction>();
    model->Read(path);

    return model;
}

void write_model(const model_ptr& model, const path& output_path) {
    remove_all(output_path);
    create_directories(output_path);

    model->Write(output_path);
}

vector<Image> get_images(const Reconstruction& model) {
    vector<Image> images;
    auto image_count = model.NumImages();
    images.resize(image_count);
    auto image_map = model.Images();

    for (size_t i = 1; i <= image_count; ++i) {
        auto image = image_map[i];
        images[i - 1] = image;
    }
    return images;
}

vector<point_pair> get_points(const Reconstruction& model) {
    auto point_map = model.Points3D();

    vector<point_pair> point_pairs(point_map.begin(), point_map.end());

    std::ranges::sort(point_pairs.begin(), point_pairs.end(), std::ranges::greater(), &point_pair::first);

    return point_pairs;
}

scored_point_map get_scored_points(const Reconstruction& model) {
    auto model_points = get_points(model);
    map<point_id_t, scored_point> result;
    for (const auto& point_pair: model_points) {
        auto point_id = point_pair.first;
        auto point = point_pair.second;
        result.insert(make_pair(point_id, scored_point(point.XYZ(), point_id, 0)));
    }
    return result;
}
