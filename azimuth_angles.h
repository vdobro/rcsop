#pragma once

#include <regex>
#include <memory>
#include <exception>
#include <filesystem>

#include "utils/types.h"
#include "utils/utils.h"
#include "utils/model.h"
#include "utils/projection.h"
#include "utils/az_data.h"
#include "utils/relative_points.h"

#include "rendering/render_points.h"

#include "options.h"

shared_ptr<point_display_payload> display_azimuth(const model_ptr& model,
                                                  const path& image_path,
                                                  const path& data_path,
                                                  const path& output_path);

void render_to_files(const point_display_payload& point_payload,
                     const path& output_path);