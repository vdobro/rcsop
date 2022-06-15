#ifndef RCSOP_LAUNCHER_AZIMUTH_RCS_PLOTTER_H
#define RCSOP_LAUNCHER_AZIMUTH_RCS_PLOTTER_H

#include <filesystem>
using std::filesystem::path;

#include "input_data_collector.h"

void azimuth_rcs_plotter(const shared_ptr<InputDataCollector>& inputs,
                         const path& output_path);

#endif //RCSOP_LAUNCHER_AZIMUTH_RCS_PLOTTER_H
