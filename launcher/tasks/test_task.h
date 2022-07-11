#ifndef RCSOP_LAUNCHER_TEST_TASK_H
#define RCSOP_LAUNCHER_TEST_TASK_H

#include "utils/task_utils.h"

namespace rcsop::launcher::tasks {
    using rcsop::data::InputDataCollector;
    using rcsop::launcher::utils::task_options;

    void test_task(const InputDataCollector& inputs,
                   const task_options& options);
}
#endif //RCSOP_LAUNCHER_TEST_TASK_H
