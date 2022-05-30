#ifndef SFM_COLORING_COMMON_SCORED_POINT_H
#define SFM_COLORING_COMMON_SCORED_POINT_H

#include "utils/types.h"

typedef std::pair<point_id_t, Vector3d> point_pair;

class scored_point {

private:
    point_id_t _point_id = -1;
    Vector3d _position = Vector3d::Zero();
    double _score = 0;

public:
    explicit scored_point() = default;

    explicit scored_point(const point_pair& base);

    explicit scored_point(Vector3d position, ulong id);

    explicit scored_point(Vector3d position, ulong id, double score);

    void increment_score(double value);

    [[nodiscard]] Vector3d position() const;

    [[nodiscard]] double score() const;

    [[nodiscard]] point_id_t id() const;

    [[nodiscard]] double score_to_dB() const;
};

typedef map<point_id_t, scored_point> scored_point_map;

#endif //SFM_COLORING_COMMON_SCORED_POINT_H