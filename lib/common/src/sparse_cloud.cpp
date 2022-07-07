#include "sparse_cloud.h"

#include "utils/colmap.h"
#include "utils/mapping.h"

SparseCloud::SparseCloud(const path& model_path) {
    this->model_path = model_path;
    reload();

    vector<Image> images;
    for (auto& image: reconstruction->Images()) {
        images.push_back(image.second);
    }
    std::sort(std::execution::par_unseq,
              images.begin(), images.end(),
              [](const Image& a, const Image& b) {
                  return a.Name() < b.Name();
              });
    for (auto& image: images) {
        camera camera(image, *reconstruction);
        this->cameras.push_back(camera);
        this->camera_map.insert(make_pair(camera.id(), camera));
    }
}

void SparseCloud::reload() {
    this->reconstruction = std::make_unique<Reconstruction>();
    this->reconstruction->Read(model_path);
}

void SparseCloud::save(const path& output_path) {
    if (!exists(output_path)) {
        create_directories(output_path);
    }
    path cameras_path{output_path / "cameras.bin"};
    path images_path{output_path / "images_path.bin"};
    path points_path{output_path / "points3D.bin"};
    remove(cameras_path);
    remove(images_path);
    remove(points_path);

    reconstruction->Write(output_path);
}

std::vector<camera> SparseCloud::get_cameras() const {
    return this->cameras;
}

std::vector<point_pair> SparseCloud::get_point_pairs() const {
    auto point_map = reconstruction->Points3D();

    vector<point_pair> point_pairs;
    for (const auto& item: point_map) {
        auto pair = make_pair(item.first, item.second.XYZ());
        point_pairs.push_back(pair);
    }

    std::ranges::sort(point_pairs.begin(), point_pairs.end(), std::ranges::greater(), &point_pair::first);

    return point_pairs;
}

shared_ptr<vector<ScoredPoint>> SparseCloud::get_scored_points() const {
    auto model_points = get_point_pairs();
    auto result = make_shared<vector<ScoredPoint>>();
    for (const auto& point_pair: model_points) {
        auto point_id = point_pair.first;
        auto point = point_pair.second;
        result->emplace_back(point, point_id, 0);
    }
    return result;
}

void SparseCloud::filter_points(const std::function<bool(const Vector3d&)>& predicate_to_keep) {
    auto points = reconstruction->Points3D();

    for (const auto& point_with_id: points) {
        auto point_id = point_with_id.first;
        auto point = point_with_id.second;
        if (!predicate_to_keep(point.XYZ())) {
            reconstruction->DeletePoint3D(point_id);
        }
    }
}

void SparseCloud::add_point(const Vector3d& point, const Vector3ub& color) {
    reconstruction->AddPoint3D(point, colmap::Track(), color);
}

void SparseCloud::set_point_color(point_id_t point_id, const Vector3ub& color) {
    reconstruction->Point3D(point_id).Color() = color;
}
