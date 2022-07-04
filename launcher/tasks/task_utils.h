#ifndef RCSOP_LAUNCHER_TASK_UTILS_H
#define RCSOP_LAUNCHER_TASK_UTILS_H

#include <filesystem>

using std::filesystem::path;

#include "input_data_collector.h"

struct task_options {
    path input_path;
    path output_path;
    double camera_distance_to_origin;
};

using launcher_task = std::function<void(const shared_ptr<InputDataCollector>,
                                         const task_options&)>;

#endif //RCSOP_LAUNCHER_TASK_UTILS_H
