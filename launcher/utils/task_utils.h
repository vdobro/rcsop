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
    using rcsop::common::height_t;

    using rcsop::data::InputDataCollector;

    using rcsop::rendering::ObserverRenderer;

    struct task_options {
        path input_path;
        path output_path;
        ScoreRange db_range;
        camera_options camera;
        rcsop::rendering::rendering_options rendering;
    };

    using launcher_task = std::function<void(const InputDataCollector&, const task_options&)>;

    void batch_render(vector<ObserverRenderer>& renderers,
                      const task_options& options,
                      const vector<height_t>& observer_heights);
}

#endif //RCSOP_LAUNCHER_TASK_UTILS_H
