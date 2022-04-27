#pragma once

#include <string>
#include <memory>
#include <vector>

#include "colmap/base/reconstruction.h"
#include "colmap/util/types.h"

using std::string;
using std::shared_ptr;
using std::vector;

using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Vector3ub;

using colmap::Reconstruction;
using colmap::Image;
using colmap::Camera;
using colmap::Point3D;

typedef shared_ptr<Reconstruction> model_ptr;