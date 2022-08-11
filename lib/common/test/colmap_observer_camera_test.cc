#include <gtest/gtest.h>
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Gt;
using ::testing::Return;

#include <utility>

#include "utils/types.h"
#include "utils/mapping.h"
#include "utils/sparse.h"
#include "colmap_observer_camera.h"

using rcsop::common::utils::points::vec3;

const double STANDARD_ERROR = 2E-8;

TEST(ColmapObserverCameraShould, MapToWorldToLocalBeEqual) {
    //TODO:
}