#include "scored_cloud.h"

ScoredCloud::ScoredCloud(const Observer& observer, const vector <scored_point>& points) :
        _observer(observer),
        _points(points) {}

Observer ScoredCloud::observer() const {
    return _observer;
}

vector<scored_point> ScoredCloud::points() const {
    return this->_points;
}
