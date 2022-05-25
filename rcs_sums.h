#pragma once

#include <execution>

#include "utils/types.h"
#include "utils/projection.h"
#include "utils/mat_reader.h"
#include "utils/relative_points.h"

#include "rendering/render_points.h"

#include "options.h"

void accumulate_rcs(const model_ptr& model,
                    const rcs_data& rcs_data,
                    const path& input_path,
                    const path& output_path);

void accumulate_azimuth(const model_ptr& model,
                        const rcs_data& rcs_data,
                        const path& input_path,
                        const path& output_path);