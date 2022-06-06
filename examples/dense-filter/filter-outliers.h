#ifndef SFM_COLORS_FILTER_OUTLIERS_H
#define SFM_COLORS_FILTER_OUTLIERS_H

#include <filesystem>

using std::filesystem::path;

void filter_outliers(const path& input_file,
                     const path& output_file);

#endif //SFM_COLORS_FILTER_OUTLIERS_H
