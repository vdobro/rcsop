#ifndef RCSOP_LAUNCHER_TASK_UTILS_H
#define RCSOP_LAUNCHER_TASK_UTILS_H

#include <filesystem>

#include "input_data_collector.h"
#include "rendering_options.h"
#include "observer_renderer.h"

namespace rcsop::launcher::utils {
    using std::filesystem::path;

    using rcsop::common::ScoreRange;
    using rcsop::common::camera_options;
    using rcsop::common::OutputDataWriter;
    using rcsop::common::height_t;

    using rcsop::data::InputDataCollector;

    using rcsop::rendering::ObserverRenderer;
    using rcsop::rendering::rendering_options;

    struct task_options {
        string task_name;
        path input_path;
        path output_path;
        bool prefilter_data;
        double vertical_spread;
        ScoreRange db_range;
        camera_options camera;
        rendering_options rendering;
    };

    using launcher_task = std::function<void(const InputDataCollector&, const task_options&)>;

    void batch_output(vector<shared_ptr<OutputDataWriter>>& output_writers,
                      const task_options& options,
                      const vector<height_t>& observer_heights);
}

#endif //RCSOP_LAUNCHER_TASK_UTILS_H
