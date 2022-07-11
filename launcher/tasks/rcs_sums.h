#ifndef RCSOP_LAUNCHER_RCS_SUMS_H
#define RCSOP_LAUNCHER_RCS_SUMS_H

#include "utils/task_utils.h"

namespace rcsop::launcher::tasks {
    using rcsop::data::InputDataCollector;
    using rcsop::launcher::utils::task_options;

    void accumulate_rcs(const InputDataCollector& inputs,
                        const task_options& options);

    void accumulate_azimuth(const InputDataCollector& inputs,
                            const task_options& options);
}
#endif //RCSOP_LAUNCHER_RCS_SUMS_H
