#pragma once

#include <regex>
#include <memory>
#include <exception>
#include <filesystem>

#include "utils/types.h"
#include "utils/utils.h"
#include "utils/model.h"

#include "utils/az_data.h"

using std::filesystem::path;

void display_azimuth(const model_ptr& model,
                     const path& image_path,
                     const path& data_path,
                     const path& output_path);