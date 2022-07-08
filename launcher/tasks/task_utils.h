#ifndef RCSOP_LAUNCHER_TASK_UTILS_H
#define RCSOP_LAUNCHER_TASK_UTILS_H

#include <filesystem>

using std::filesystem::path;

#include "input_data_collector.h"
#include "rendering_options.h"

struct task_options {
    path input_path;
    path output_path;
    ScoreRange db_range;
    camera_options camera;
    rendering_options rendering;
};

using launcher_task = std::function<void(const InputDataCollector&, const task_options&)>;

#endif //RCSOP_LAUNCHER_TASK_UTILS_H
