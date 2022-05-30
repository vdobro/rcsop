#pragma once

#include <execution>

#include "../../common/utils/types.h"
#include "../../data/rcs_data.h"
#include "../../data/relative_points.h"

#include "../../rendering/render_points.h"

#include "../../options.h"

void accumulate_rcs(const sparse_cloud& model,
                    const rcs_data& rcs_data,
                    const path& input_path,
                    const path& output_path);

void accumulate_azimuth(const sparse_cloud& model,
                        const rcs_data& rcs_data,
                        const path& input_path,
                        const path& output_path);