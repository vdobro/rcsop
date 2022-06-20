#include "rcs_sums.h"

#include "utils/chronometer.h"
#include "utils/mapping.h"
#include "utils/gauss.h"
#include "utils/logging.h"
#include "utils/point_scoring.h"

#include "observed_point.h"
#include "point_cloud_provider.h"
#include "default_options.h"
#include "observer_provider.h"
#include "scored_cloud.h"
#include "observer_renderer.h"
#include "options.h"

using namespace sfm::rendering;

#define HORIZONTAL_ANGLE 25
#define VERTICAL_ANGLE 15
#define STANDARD_DEVIATION 0.2

#define HORIZONTAL_SPREAD (HORIZONTAL_ANGLE / 2.0)
#define VERTICAL_SPREAD (VERTICAL_ANGLE / 2.0)

const static gauss_options distribution_options = {
        .sigma = STANDARD_DEVIATION,
        .integral_factor = get_gauss_integral_factor(STANDARD_DEVIATION),
        .x_scale = HORIZONTAL_SPREAD,
        .y_scale = VERTICAL_SPREAD,
};

void accumulate_rcs(const shared_ptr<InputDataCollector>& input,
                    const path& output_path) {
    //TODO: WIP
}

void accumulate_azimuth(const shared_ptr<InputDataCollector>& inputs,
                        const path& output_path) {
    /*
     //TODO WIP: adapter for BasicRcsMap or refactoring
    const shared_ptr<BasicRcsMap> rcs_data = inputs->data<SIMPLE_RCS_MAT>(false);
    const auto scored_payload = score_points(inputs, rcs_data, COLOR_MAP);
    const auto& points = scored_payload->point_clouds;
    const auto& colormap = scored_payload->colormap;

    auto payload_size = points.size();
    for (size_t index = 1; index <= payload_size; index++) {
        ScoredCloud scored_cloud = points[index - 1];
        ObserverRenderer renderer(scored_cloud);
        renderer.render(output_path, colormap, construct_log_prefix(index, payload_size));
    }*/
}
