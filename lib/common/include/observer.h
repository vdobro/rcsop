#ifndef RCSOP_COMMON_OBSERVER_H
#define RCSOP_COMMON_OBSERVER_H

#include <optional>

using std::optional;

#include "utils/points.h"

#include "camera.h"
#include "observer_position.h"
#include "observed_point.h"

struct CameraCorrectionParams {
    double pitch = 0.;
    height_t default_height = 40;
};

class Observer {
private:
    optional<ObserverPosition> _position;
    path _source_filepath;
    camera _camera;

    double _world_scale = 1;
    double _height_offset = 0;
    Eigen::Transform<double, 3, Eigen::Affine> _correction_transform;

    [[nodiscard]] Vector3d get_right() const;

    [[nodiscard]] Vector3d get_up() const;

public:
    explicit Observer(optional<ObserverPosition> camera_position,
                      path filepath,
                      camera camera,
                      double world_scale,
                      CameraCorrectionParams camera_correction);

    [[nodiscard]] ObserverPosition position() const;
    [[nodiscard]] bool has_position() const;

    [[nodiscard]] path source_image_path() const;

    [[nodiscard]] camera native_camera() const;

    [[nodiscard]] shared_ptr<vector<observed_point>> observe_points(const vector<ScoredPoint>& camera_points) const;

};

#endif //RCSOP_COMMON_OBSERVER_H
