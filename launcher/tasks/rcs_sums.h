#ifndef RCSOP_LAUNCHER_RCS_SUMS_H
#define RCSOP_LAUNCHER_RCS_SUMS_H

#include "task_utils.h"
#include "sparse_cloud.h"

void accumulate_rcs(const InputDataCollector& inputs,
                    const task_options& options);

void accumulate_azimuth(const InputDataCollector& inputs,
                        const task_options& options);

#endif //RCSOP_LAUNCHER_RCS_SUMS_H
