#include "model_camera.h"

#include "utils/sparse.h"
#include "utils/mapping.h"

namespace rcsop::common {
    using rcsop::common::utils::map_vec;

    using rcsop::common::utils::sparse::Image;
    using rcsop::common::utils::sparse::Reconstruction;
    using rcsop::common::utils::points::vec2;
    using rcsop::common::utils::points::vec3;

    ModelCamera::ModelCamera(const Image& image,
                             const Reconstruction& model) : _model_image(image) {
        this->_model_camera = model.Camera(image.CameraId());
    }

    vec3 ModelCamera::transform_to_world(const vec3& local_coordinates) const {
        return _model_image.InverseProjectionMatrix() * local_coordinates.homogeneous();
    }

    vec3 ModelCamera::transform_to_local(const vec3& world_coordinates) const {
        return _model_image.ProjectionMatrix() * world_coordinates.homogeneous();
    }

    vec3 ModelCamera::position() const {
        return transform_to_world(vec3::Zero());
    }

    camera_id_t ModelCamera::id() const {
        return _model_image.ImageId();
    }

    vec2 ModelCamera::project_from_image(const vec2& point) const {
        return _model_camera.WorldToImage(point);
    }

    size_t ModelCamera::image_width() const {
        return _model_camera.Width();
    }

    size_t ModelCamera::image_height() const {
        return _model_camera.Height();
    }

    string ModelCamera::get_name() const {
        return _model_image.Name();
    }

    vector<ImagePoint> ModelCamera::project_to_image(
            const vector<ScoredPoint>& points) const {
        auto camera_position = this->position();
        auto image_projection_matrix = _model_image.ProjectionMatrix();

        return utils::map_vec<ScoredPoint, ImagePoint>(points, [camera_position, image_projection_matrix]
                (const ScoredPoint& point) -> ImagePoint {
            auto position =  point.position();
            auto score = point.score_to_dB();
            auto position_normalized = (image_projection_matrix * position.homogeneous()).hnormalized();
            auto distance_to_camera = (camera_position - position).norm();
            auto projected_point = ImagePoint(position_normalized, distance_to_camera, score);
            return projected_point;
        });
    }

    string ModelCamera::get_last_name_segment() const {
        std::stringstream name_stream(get_name());
        vector<string> segment_list;
        string segment;
        while (std::getline(name_stream, segment, std::filesystem::path::preferred_separator)) {
            segment_list.push_back(segment);
        }
        auto last_segment = segment_list.at(segment_list.size() - 1);
        return last_segment;
    }
}
