#ifndef RCSOP_LAUNCHER_POINT_SCORING_H
#define RCSOP_LAUNCHER_POINT_SCORING_H

#include "utils/types.h"
#include "scored_cloud.h"
#include "input_data_collector.h"

#include "colors.h"

using namespace sfm::rendering;

struct ScoredCloudPayload {
    vector<ScoredCloud> point_clouds;
    global_colormap_func colormap;
};

using observed_factor_func = function<double(const observed_point&)>;

shared_ptr<ScoredCloudPayload> score_points(
        const shared_ptr<InputDataCollector>& inputs,
        const shared_ptr<AbstractDataCollection>& data,
        const double distance_to_origin,
        const observed_factor_func& factor_func,
        const ScoreRange& range_limits,
        const global_colormap_func& colormap_func);

shared_ptr<ScoredCloudPayload> score_points(
        const shared_ptr<InputDataCollector>& inputs,
        const shared_ptr<AbstractDataCollection>& data,
        const double distance_to_origin,
        const ScoreRange& range_limits,
        const global_colormap_func& colormap_func);

#endif //RCSOP_LAUNCHER_POINT_SCORING_H
