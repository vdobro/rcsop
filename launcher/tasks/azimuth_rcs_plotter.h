#ifndef RCSOP_LAUNCHER_AZIMUTH_RCS_PLOTTER_H
#define RCSOP_LAUNCHER_AZIMUTH_RCS_PLOTTER_H

#include "utils/task_utils.h"

namespace rcsop::launcher::tasks {

    void azimuth_rcs_plotter(const InputDataCollector& inputs,
                             const rcsop::launcher::utils::task_options& options);

}
#endif //RCSOP_LAUNCHER_AZIMUTH_RCS_PLOTTER_H
