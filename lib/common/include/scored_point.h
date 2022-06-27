#ifndef RCSOP_COMMON_SCORED_POINT_H
#define RCSOP_COMMON_SCORED_POINT_H

#include "utils/types.h"
#include "utils/points.h"

struct ScoreRange {
    double min;
    double max;
};

class ScoredPoint {

private:
    point_id_t _point_id = -1;
    Vector3d _position = Vector3d::Zero();
    double _score = 0;

public:
    explicit ScoredPoint() = default;

    explicit ScoredPoint(Vector3d position, point_id_t id, double score = 0);

    [[nodiscard]] Vector3d position() const;

    [[nodiscard]] double score() const;

    [[nodiscard]] point_id_t id() const;

    [[nodiscard]] double score_to_dB() const;

    static ScoreRange get_score_range(const vector<ScoredPoint>& points);

};

#endif //RCSOP_COMMON_SCORED_POINT_H