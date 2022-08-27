#ifndef RCSOP_LAUNCHER_POINT_SCORING_H
#define RCSOP_LAUNCHER_POINT_SCORING_H

#include "utils/types.h"
#include "utils/task_utils.h"

#include "scored_cloud.h"
#include "input_data_collector.h"
#include "observed_point.h"
#include "observer.h"

#include "colors.h"

namespace rcsop::launcher::utils {
    using rcsop::common::ScoredCloud;
    using rcsop::common::observed_point;
    using rcsop::data::AbstractDataCollection;
    using rcsop::common::multiple_scored_cloud_payload;
    using rcsop::common::data_observer_translation;
    using rcsop::common::observed_factor_func;
    using rcsop::common::coloring::global_colormap_func;

    double identity_factor(const observed_point& point);

    struct data_with_observer_options {
        data_observer_translation observer_options;
        shared_ptr<AbstractDataCollection> data_collection;
    };

    auto score_points(
            const InputDataCollector& inputs,
            const vector<data_with_observer_options>& data,
            const task_options& task_options,
            const global_colormap_func& color_map_func,
            const observed_factor_func& factor_func = &identity_factor) -> shared_ptr<multiple_scored_cloud_payload const>;

}

#endif //RCSOP_LAUNCHER_POINT_SCORING_H
