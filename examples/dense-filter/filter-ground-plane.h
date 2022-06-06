#ifndef SFM_COLORS_FILTER_GROUND_PLANE_H
#define SFM_COLORS_FILTER_GROUND_PLANE_H

#include <filesystem>

using std::filesystem::path;

void separate_plane(const path& input_file,
                    const path& output_file);

#endif //SFM_COLORS_FILTER_GROUND_PLANE_H
