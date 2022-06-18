#ifndef RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H
#define RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H

#include "scored_point.h"
#include "observer.h"

class ScoredCloud {
private:
    const Observer& _observer;
    const vector<ScoredPoint>& _points;
public:
    explicit ScoredCloud(const Observer& observer,
                                 const vector<ScoredPoint>& points);

    Observer observer() const;

    vector<ScoredPoint> points() const;
};


#endif //RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H
