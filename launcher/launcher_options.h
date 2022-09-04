#ifndef RCSOP_LAUNCHER_LAUNCHER_OPTIONS_H
#define RCSOP_LAUNCHER_LAUNCHER_OPTIONS_H

#include "utils/task_utils.h"

namespace rcsop::launcher {
    using rcsop::launcher::utils::task_options;
    using rcsop::launcher::utils::launcher_task;

    [[nodiscard]] task_options parse_and_validate(
            int argc, char* argv[],
            const map<string, launcher_task>& available_tasks);
}
#endif //RCSOP_LAUNCHER_LAUNCHER_OPTIONS_H
