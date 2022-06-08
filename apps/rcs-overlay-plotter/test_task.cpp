#include "test_task.h"

#include <iostream>

using std::clog;
using std::endl;

void dummy_task(const InputDataCollector& inputs,
                const path& output_path) {
    clog << "Image file paths: " << endl;
    for (const auto& image_path : inputs.image_paths()) {
        clog << image_path << endl;
    }

    clog << "RCS azimuth data heights: " << endl;
    for (const auto& height_data : inputs.rcs_azimuth_data()) {
        clog << height_data.first << endl;
    }

    clog << "Test task done" << endl;
}
