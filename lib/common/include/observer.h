#ifndef RCSOP_COMMON_OBSERVER_H
#define RCSOP_COMMON_OBSERVER_H

#include "observer_position.h"
#include "camera.h"
#include "observed_point.h"

class Observer {
private:
    const ObserverPosition _position;
    const path _source_filepath;
    const camera _camera;
    const double _worldScale;

    [[nodiscard]] Vector3d get_right() const;

    [[nodiscard]] Vector3d get_up() const;

public:
    explicit Observer(const ObserverPosition& camera_position,
                      const camera& camera,
                      path filepath,
                      double world_scale);

    [[nodiscard]] ObserverPosition position() const;

    [[nodiscard]] path source_image_path() const;

    [[nodiscard]] camera native_camera() const;

    [[nodiscard]] vector<observed_point> observe_points(const ScoredPointMap& camera_points) const;
};

#endif //RCSOP_COMMON_OBSERVER_H