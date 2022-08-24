#include "sparse_cloud.h"

#include "utils/sparse.h"
#include "utils/mapping.h"

namespace rcsop::common {
    using rcsop::common::utils::sparse::Image;

    SparseCloud::SparseCloud(const path& model_path) : BasePointCloud(model_path) {
        reload();

        vector<Image> images;
        for (auto& [_, image]: reconstruction->Images()) {
            images.push_back(image);
        }
        std::sort(std::execution::par_unseq,
                  images.begin(), images.end(),
                  [](const Image& a, const Image& b) {
                      return a.Name() < b.Name();
                  });
        for (auto& image: images) {
            ModelCamera camera(image, *reconstruction);
            this->cameras.push_back(camera);
        }
    }

    void SparseCloud::reload() {
        this->reconstruction = make_unique<Reconstruction>();
        this->reconstruction->Read(this->model_path());
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

    std::vector<ModelCamera> SparseCloud::get_cameras() const {
        return this->cameras;
    }

    shared_ptr<vector<IdPoint>> SparseCloud::get_points() const {
        auto point_map = reconstruction->Points3D();
        auto result = make_shared<vector<IdPoint>>();
        for (const auto& [point_id, point]: point_map) {
            result->emplace_back(point_id, point.XYZ());
        }

        std::ranges::sort(result->begin(), result->end(), std::ranges::greater(), &IdPoint::id);
        return result;
    }

    void SparseCloud::filter_points(const std::function<bool(const vec3&)>& predicate_to_keep) {
        auto points = reconstruction->Points3D();

        for (const auto& [point_id, point]: points) {
            if (!predicate_to_keep(point.XYZ())) {
                reconstruction->DeletePoint3D(point_id);
            }
        }
    }

    static Eigen::Vector3ub convert_color(const color_vec& color) {
        return color.head(3);
    }

    void SparseCloud::add_point(const vec3& point, const color_vec& color) {
        reconstruction->AddPoint3D(point, colmap::Track(), convert_color(color));
    }

    void SparseCloud::write(const path& output_path) {
        create_directories(output_path);
        reconstruction->WriteBinary(output_path);
    }

    void SparseCloud::set_point_color(point_id_t point_id, const color_vec& color) {
        reconstruction->Point3D(point_id).Color() = convert_color(color);
    }

    size_t SparseCloud::point_count() const {
        return this->reconstruction->NumPoints3D();
    }

    void SparseCloud::purge_cameras(const camera_id_t camera_to_keep) {
        auto points = get_points();
        map<point_id_t, color_vec> point_colors;

        auto native_points = reconstruction->Points3D();
        for (const auto& point: *points) {
            auto color = native_points.at(point.id()).Color();
            point_colors.insert(make_pair(point.id(), color.homogeneous()));
        }

        for (const auto& camera: get_cameras()) {
            if (camera.id() == camera_to_keep) {
                continue;
            }
            reconstruction->DeRegisterImage(camera.id());
        }

        for (const auto& point: *points) {
            auto color = point_colors.at(point.id());
            reconstruction->AddPoint3D(point.position(), colmap::Track{}, convert_color(color));
        }
    }
}
