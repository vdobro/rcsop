#ifndef RCSOP_LAUNCHER_RCS_SUMS_H
#define RCSOP_LAUNCHER_RCS_SUMS_H

#include "sparse_cloud.h"
#include "input_data_collector.h"

void accumulate_rcs(const shared_ptr<InputDataCollector>& inputs,
                    const path& output_path);

void accumulate_azimuth(const shared_ptr<InputDataCollector>& inputs,
                        const path& output_path);

#endif //RCSOP_LAUNCHER_RCS_SUMS_H
