#pragma once

#include <utility>

#include "utils/types.h"

class scored_point {

private:
    point_id_t _point_id = -1;
    Vector3d _position = Vector3d::Zero();
    double _score = 0;

public:
    explicit scored_point() = default;

    explicit scored_point(const point_pair& base);

    explicit scored_point(Vector3d position, ulong id) : _point_id(id), _position(std::move(position)) {}

    explicit scored_point(Vector3d position, ulong id, double score) : _point_id(id), _position(std::move(position)), _score(score) {}

    void increment_score(double value);

    [[nodiscard]] Vector3d position() const;

    [[nodiscard]] double score() const;

    [[nodiscard]] point_id_t id() const;

    [[nodiscard]] double score_to_dB() const;
};
