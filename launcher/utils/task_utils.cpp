#include "utils/task_utils.h"

namespace rcsop::launcher::utils {
    using std::for_each;
    using std::filesystem::create_directories;

    using rcsop::data::ModelWriter;

    using rcsop::common::utils::logging::construct_log_prefix;

    void batch_output(vector<shared_ptr<OutputDataWriter>>& output_writers,
                      const task_options& options,
                      const vector<height_t>& observer_heights) {
        map<height_t, path> height_folders;
        for (auto height: observer_heights) {
            path height_path{options.output_path / (std::to_string(height) + "cm")};
            height_folders.insert(make_pair(height, height_path));
        }
        if (height_folders.size() > 1) {
            for_each(height_folders.begin(), height_folders.end(), [](const auto& height_with_path) {
                const auto [height, path] = height_with_path;
                create_directories(path);
            });
        }
        auto renderer_indexes = common::utils::get_indices(output_writers);
        std::for_each(
                std::execution::seq,
                renderer_indexes.cbegin(),
                renderer_indexes.cend(),
                [&options, &output_writers, &height_folders](const auto renderer_index) {
                    auto output_writer = output_writers.at(renderer_index);

                    const auto place_in_separate_folder =
                            height_folders.size() > 1 && output_writer->observer_has_position();

                    path output_path = place_in_separate_folder
                                       ? height_folders.at(output_writer->observer_height())
                                       : options.output_path;
                    path output_processor_folder = output_path / output_writer->path_prefix();
                    create_directories(output_processor_folder);
                    output_writer->write(output_processor_folder,
                                         construct_log_prefix(renderer_index + 1, output_writers.size()));
                });
    }
}