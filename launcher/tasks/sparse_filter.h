#ifndef RCSOP_LAUNCHER_SPARSE_FILTER_H
#define RCSOP_LAUNCHER_SPARSE_FILTER_H

#include "utils/task_utils.h"

namespace rcsop::launcher::tasks {
    void sparse_filter(const InputDataCollector& inputs,
                       const rcsop::launcher::utils::task_options& options);
}
#endif //RCSOP_LAUNCHER_SPARSE_FILTER_H
