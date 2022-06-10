#ifndef RCS_OVERLAY_PLOTTER_TEST_TASK_H
#define RCS_OVERLAY_PLOTTER_TEST_TASK_H

#include <filesystem>
using std::filesystem::path;

#include "input_data_collector.h"

void dummy_task(const InputDataCollector& inputs,
                const path& output_path);

#endif //RCS_OVERLAY_PLOTTER_TEST_TASK_H
