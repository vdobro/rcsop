#ifndef RCSOP_LAUNCHER_POINT_SCORING_H
#define RCSOP_LAUNCHER_POINT_SCORING_H

#include "utils/types.h"
#include "tasks/task_utils.h"

#include "scored_cloud.h"
#include "input_data_collector.h"

#include "colors.h"

using namespace sfm::rendering;

struct scored_cloud_payload {
    vector<ScoredCloud> point_clouds;
    global_colormap_func colormap;

    [[nodiscard]] vector<height_t> observer_heights() const {
        std::set<height_t> heights;
        for (auto cloud : point_clouds) {
            if (cloud.observer().has_position())
            heights.insert(cloud.observer().position().height);
        }
        vector<height_t> result;
        std::copy(heights.begin(), heights.end(), std::back_inserter(result));
        return result;
    }
};

using observed_factor_func = function<double(const observed_point&)>;

double identity_factor(const observed_point& point);

shared_ptr<scored_cloud_payload> score_points(
        const InputDataCollector& inputs,
        const AbstractDataCollection& data,
        const task_options& task_options,
        const global_colormap_func& colormap_func,
        const observed_factor_func& factor_func = &identity_factor);

#endif //RCSOP_LAUNCHER_POINT_SCORING_H
