#include "scored_cloud.h"

#include <utility>

ScoredCloud::ScoredCloud(Observer observer,
                         shared_ptr<vector<ScoredPoint>> points) :
        _observer(std::move(observer)),
        _points(std::move(points)) {}

Observer ScoredCloud::observer() const {
    return _observer;
}

shared_ptr<vector<ScoredPoint>> ScoredCloud::points() const {
    return this->_points;
}
