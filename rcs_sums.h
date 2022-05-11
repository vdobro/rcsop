#pragma once

#include "utils/utils.h"
#include "utils/mat_reader.h"
#include "rendering/render_points.h"

#undef LEGACY_GAUSS

void accumulate_rcs(const model_ptr& model,
                    const rcs_data& rcs_data,
                    const string& input_path,
                    const string& output_path);

void accumulate_azimuth(const model_ptr& model,
                        const rcs_data& rcs_data,
                        const string& input_path,
                        const string& output_path);