#ifndef RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H
#define RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H

#include "utils/types.h"

#include "scored_point.h"
#include "observer.h"
#include "colors.h"

namespace rcsop::common {
    using rcsop::common::coloring::global_colormap_func;

    class ScoredCloud {
    private:
        Observer _observer;
        shared_ptr<vector<ScoredPoint>> _points;
    public:
        ScoredCloud(const Observer& observer, shared_ptr<vector<ScoredPoint>> points);

        [[nodiscard]] const Observer& observer() const;

        [[nodiscard]] shared_ptr<vector<ScoredPoint>> points() const;
    };

    struct scored_cloud_payload {
        ScoredCloud point_cloud;
        global_colormap_func colormap;
    };

    struct multiple_scored_cloud_payload {
        vector<ScoredCloud> point_clouds;
        global_colormap_func colormap;

        [[nodiscard]] auto observer_heights() const -> vector<height_t>;

        [[nodiscard]] auto extract_single_payloads() const -> vector<scored_cloud_payload>;
    };
}

#endif //RCSOP_COMMON_OBSERVED_SCORED_CLOUD_H
