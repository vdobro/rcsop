#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include <map>
#include <cmath>
#include <utility>

#include <Eigen/Core>

using std::string;
using std::shared_ptr;
using std::map;
using std::pair;

using Eigen::Vector2d;
using Eigen::Vector3d;

typedef long rcs_height_t;
typedef ulong point_id_t;

using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::remove;