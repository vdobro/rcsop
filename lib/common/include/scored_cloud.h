#ifndef RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H
#define RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H

#include "utils/types.h"

#include "scored_point.h"
#include "observer.h"

namespace rcsop::common {

    class ScoredCloud {
    private:
        const Observer _observer;
        const shared_ptr<vector<ScoredPoint>> _points;
    public:
        explicit ScoredCloud(Observer observer,
                             shared_ptr<vector<ScoredPoint>> points);

        [[nodiscard]] const Observer& observer() const;

        [[nodiscard]] shared_ptr<vector<ScoredPoint>> points() const;
    };
}

#endif //RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H
