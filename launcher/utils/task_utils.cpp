#include "utils/task_utils.h"

using std::for_each;
using std::filesystem::create_directories;

using rcsop::rendering::ObserverRenderer;
using rcsop::launcher::utils::task_options;

void rcsop::launcher::utils::batch_render(vector<ObserverRenderer>& renderers,
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
    std::ranges::iota_view renderer_indexes(0ul, renderers.size());
    std::for_each(
            std::execution::seq,
            renderer_indexes.begin(),
            renderer_indexes.end(),
            [&options, &renderers, &height_folders](const auto renderer_index) {
                ObserverRenderer& renderer = renderers.at(renderer_index);

                path output_path = renderer.observer_has_position()
                                   ? height_folders.at(renderer.observer_height())
                                   : options.output_path;
                renderer.render(options.output_path, construct_log_prefix(
                        renderer_index + 1,
                        renderers.size() + 1));
            });
}