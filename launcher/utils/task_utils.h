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

    enum PointGenerator {
        MODEL_POINT_CLOUD = 1,
        BOUNDING_BOX = 2,
        DATA_PROJECTION = 4,
        MODEL_WITH_PROJECTION = MODEL_POINT_CLOUD | DATA_PROJECTION,
    };

    struct vertical_spread {
        double angle_spread;
        double normal_variance;
    };

    enum OutputFormat {
        NONE = 0,
        RENDERING = 1,
        SPARSE_MODEL = 2,

        BOTH = RENDERING | SPARSE_MODEL,
    };

    struct task_options {
        string task_name;
        path input_path;
        path output_path;
        bool prefilter_data;
        vertical_spread vertical_options;
        PointGenerator point_generator;
        size_t point_density;
        ScoreRange db_range;
        camera_options camera;
        rendering_options rendering;
        OutputFormat output_format;
    };

    using launcher_task = std::function<void(const InputDataCollector&, const task_options&)>;

    void batch_output(vector<shared_ptr<OutputDataWriter>>& output_writers,
                      const task_options& options,
                      const vector<height_t>& observer_heights);
}

#endif //RCSOP_LAUNCHER_TASK_UTILS_H
