#ifndef RCSOP_LAUNCHER_TEST_TASK_H
#define RCSOP_LAUNCHER_TEST_TASK_H

#include <filesystem>
using std::filesystem::path;

#include "input_data_collector.h"

void dummy_task(const shared_ptr<InputDataCollector>& inputs,
                const path& output_path);

#endif //RCSOP_LAUNCHER_TEST_TASK_H
