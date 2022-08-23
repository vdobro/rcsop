#ifndef RCSOP_COMMON_BASE_POINT_CLOUD_H
#define RCSOP_COMMON_BASE_POINT_CLOUD_H

#include "utils/types.h"
#include "utils/sparse.h"
#include "camera.h"

#include "id_point.h"

namespace rcsop::common {
    using rcsop::common::IdPoint;
    using rcsop::common::camera;
    using rcsop::common::utils::sparse::color_vec;

    class BasePointCloud {
    private:
        path _model_path;

    protected:
        explicit BasePointCloud(path model_path);

        virtual ~BasePointCloud() = default;

        auto model_path() -> path;

    public:
        [[nodiscard]] virtual shared_ptr<vector<IdPoint>> get_points() const = 0;

        [[nodiscard]] virtual size_t point_count() const = 0;

        virtual void add_point(const vec3& point, const color_vec& color) = 0;

        virtual void filter_points(const function<bool(const vec3&)>& predicate_to_keep) = 0;

        virtual void write(const path& output_path) = 0;

        virtual void purge_cameras(camera_id_t camera_to_keep) = 0;
    };
}

#endif //RCSOP_COMMON_BASE_POINT_CLOUD_H
