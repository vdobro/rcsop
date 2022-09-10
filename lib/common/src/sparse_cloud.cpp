#include "sparse_cloud.h"

#include "utils/sparse.h"
#include "utils/mapping.h"

namespace rcsop::common {
    using std::filesystem::is_regular_file;
    using rcsop::common::utils::sort_in_place;

    using rcsop::common::utils::sparse::Image;

    const static char* FILE_CAMERAS = "cameras.bin";
    const static char* FILE_IMAGES = "images.bin";
    const static char* FILE_POINTS = "points3D.bin";

    SparseCloud::SparseCloud(const path& model_path) : BasePointCloud(model_path) {
        reload();

        vector<Image> images;
        for (auto& [_, image]: reconstruction->Images()) {
            images.push_back(image);
        }
        utils::sort_in_place<Image, string>(images, [](const Image& a) -> string {
            return a.Name();
        });

        for (auto& image: images) {
            ModelCamera camera(image, *reconstruction);
            this->cameras.push_back(camera);
        }
    }

    bool SparseCloud::is_available_at(const path& root_path) {
        path cameras_path{root_path / FILE_CAMERAS};
        path images_path{root_path / FILE_IMAGES};
        path points_path{root_path / FILE_POINTS};

        return is_regular_file(cameras_path) && is_regular_file(images_path) && is_regular_file(points_path);
    }

    void SparseCloud::reload() {
        this->reconstruction = make_unique<Reconstruction>();
        this->reconstruction->Read(this->model_path());
    }

    void SparseCloud::save(const path& output_path) {
        if (!exists(output_path)) {
            create_directories(output_path);
        }
        path cameras_path{output_path / FILE_CAMERAS};
        path images_path{output_path / FILE_IMAGES};
        path points_path{output_path / FILE_POINTS};
        remove(cameras_path);
        remove(images_path);
        remove(points_path);

        reconstruction->Write(output_path);
    }

    std::vector<ModelCamera> SparseCloud::get_cameras() const {
        return this->cameras;
    }

    shared_ptr<vector<SimplePoint>> SparseCloud::get_points() const {
        auto point_map = reconstruction->Points3D();
        auto result = make_shared<vector<SimplePoint>>();
        for (const auto& [point_id, point]: point_map) {
            result->emplace_back(point_id, point.XYZ());
        }

        sort_in_place<SimplePoint, point_id_t>(*result, [](const SimplePoint& point) {
            return point.id();
        });
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

    void SparseCloud::add_point(const IdPoint* point,
                                const color_vec& color) {
        auto id = point->id();
        auto position = point->position();
        auto target_color = convert_color(color);
        if (reconstruction->ExistsPoint3D(id)) {
            auto& existing_point = reconstruction->Point3D(id);
            existing_point.SetXYZ(position);
            existing_point.SetColor(target_color);
        } else {
            reconstruction->AddPoint3D(position, colmap::Track(), target_color);
        }
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
