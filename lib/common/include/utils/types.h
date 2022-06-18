#ifndef RCSOP_COMMON_TYPES_H
#define RCSOP_COMMON_TYPES_H

#include <string>
#include <memory>
#include <filesystem>
#include <map>
#include <cmath>
#include <utility>
#include <vector>

#include <Eigen/Core>

using std::string;
using std::shared_ptr;
using std::map;
using std::pair;
using std::vector;

using Eigen::Vector2d;
using Eigen::Vector3d;

typedef ulong point_id_t;

using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::remove;

#endif //RCSOP_COMMON_TYPES_H
