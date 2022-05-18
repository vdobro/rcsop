#pragma once

#include <filesystem>
#include <execution>
using std::filesystem::path;

#include "utils/projection.h"
#include "utils/mat_reader.h"
#include "utils/relative_points.h"

#include "rendering/render_points.h"

#include "options.h"

void accumulate_rcs(const model_ptr& model,
                    const rcs_data& rcs_data,
                    const string& input_path,
                    const string& output_path);

void accumulate_azimuth(const model_ptr& model,
                        const rcs_data& rcs_data,
                        const string& input_path,
                        const string& output_path);