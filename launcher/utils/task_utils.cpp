#include "utils/task_utils.h"

namespace rcsop::launcher::utils {
    using std::for_each;
    using std::filesystem::create_directories;

    using rcsop::common::utils::logging::construct_log_prefix;

    void batch_render(vector<ObserverRenderer>& renderers,
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
        auto renderer_indexes = common::utils::get_indices(renderers);
        std::for_each(
                std::execution::seq,
                renderer_indexes.begin(),
                renderer_indexes.end(),
                [&options, &renderers, &height_folders](const auto renderer_index) {
                    ObserverRenderer& renderer = renderers.at(renderer_index);

                    const auto place_in_separate_folder =
                            height_folders.size() > 1 && renderer.observer_has_position();

                    path output_path = place_in_separate_folder
                                       ? height_folders.at(renderer.observer_height())
                                       : options.output_path;
                    renderer.render(output_path, construct_log_prefix(renderer_index + 1, renderers.size()));
                });
    }
}