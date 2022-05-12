#pragma once

#include <execution>

#include "utils/projection.h"
#include "utils/mat_reader.h"
#include "rendering/render_points.h"

#define CAMERA_DISTANCE 750.0

#define HORIZONTAL_ANGLE 25
#define VERTICAL_ANGLE 15
#define STANDARD_DEVIATION 0.2

// options for debugging
#undef SINGLE_PROJECTION
#define DEFAULT_CAMERA 0
#define DEFAULT_HEIGHT 40

#undef RANGE_CONSTRAINT
#define FIRST_RANGE 45
#define LAST_RANGE 49

void accumulate_rcs(const model_ptr& model,
                    const rcs_data& rcs_data,
                    const string& input_path,
                    const string& output_path);

void accumulate_azimuth(const model_ptr& model,
                        const rcs_data& rcs_data,
                        const string& input_path,
                        const string& output_path);