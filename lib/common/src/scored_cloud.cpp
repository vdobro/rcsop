#include "scored_cloud.h"

ScoredCloud::ScoredCloud(const Observer& observer, const vector <ScoredPoint>& points) :
        _observer(observer),
        _points(points) {}

Observer ScoredCloud::observer() const {
    return _observer;
}

vector<ScoredPoint> ScoredCloud::points() const {
    return this->_points;
}
