#pragma once

#include "utils/utils.h"
#include "utils/mat_reader.h"
#include "rendering/render_points.h"

#define CAMERA_DISTANCE 750.0
#define HORIZONTAL_ANGLE 30
#define VERTICAL_ANGLE 10
#define STANDARD_DEVIATION 0.3

#define SINGLE_PROJECTION
#define DEFAULT_CAMERA 10
#define DEFAULT_HEIGHT 40

void accumulate_rcs(const model_ptr& model,
                    const rcs_data& rcs_data,
                    const string& input_path,
                    const string& output_path);

void accumulate_azimuth(const model_ptr& model,
                        const rcs_data& rcs_data,
                        const string& input_path,
                        const string& output_path);