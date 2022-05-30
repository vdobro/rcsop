#pragma once

#include <execution>

#include "utils/types.h"
#include "rcs_data.h"
#include "relative_points.h"

#include "render_points.h"

#include "options.h"

void accumulate_rcs(const sparse_cloud& model,
                    const rcs_data& rcs_data,
                    const path& input_path,
                    const path& output_path);

void accumulate_azimuth(const sparse_cloud& model,
                        const rcs_data& rcs_data,
                        const path& input_path,
                        const path& output_path);