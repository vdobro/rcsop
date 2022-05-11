#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include <vector>
#include <map>
#include <cmath>

#include "colmap/base/reconstruction.h"
#include "colmap/util/types.h"

using std::string;
using std::shared_ptr;
using std::vector;
using std::map;
using std::pair;

using colmap::Reconstruction;
using colmap::Image;
using colmap::Point3D;

using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Vector3ub;

using colmap::Reconstruction;
using colmap::Image;
using colmap::Camera;
using colmap::Point3D;

typedef shared_ptr<Reconstruction> model_ptr;
typedef Eigen::ParametrizedLine<double, 3> model_line;
typedef ulong point_id_t;
typedef pair<point_id_t, Point3D> point_pair;
typedef long rcs_height_t;

using std::filesystem::create_directories;
static const auto path_separator = std::filesystem::path::preferred_separator;