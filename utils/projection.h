#pragma once

#include "utils/utils.h"
#include "utils/types.h"
#include "utils/model.h"

Vector3d transform_to_world(const Image& image, const Vector3d& image_xyz);

Vector3d get_image_position(const Image& image);

double get_world_scale(double camera_distance_to_origin, const Reconstruction& model);