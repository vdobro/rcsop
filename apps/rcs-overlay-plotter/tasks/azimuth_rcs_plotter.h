#ifndef RCS_OVERLAY_PLOTTER_SINGLE_AZIMUTH_RCS_PLOTTER_H
#define RCS_OVERLAY_PLOTTER_SINGLE_AZIMUTH_RCS_PLOTTER_H

#include <filesystem>
using std::filesystem::path;

#include "input_data_collector.h"

void azimuth_rcs_plotter(const InputDataCollector& inputs,
                         const path& output_path);

#endif //RCS_OVERLAY_PLOTTER_SINGLE_AZIMUTH_RCS_PLOTTER_H
