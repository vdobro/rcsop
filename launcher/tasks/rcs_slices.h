#ifndef RCSOP_LAUNCHER_RCS_SLICES_H
#define RCSOP_LAUNCHER_RCS_SLICES_H

#include <filesystem>
using std::filesystem::path;

#include "input_data_collector.h"

void rcs_slices(const shared_ptr<InputDataCollector>& inputs,
                const path& output_path);

#endif //RCSOP_LAUNCHER_RCS_SLICES_H
