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
    shared_ptr<AzimuthRcsMap> data = inputs.data<AZIMUTH_RCS_MAT>(false);
    for (const auto& height_data : data->heights()) {
        clog << height_data << endl;
    }

    clog << "Test task done" << endl;
}
