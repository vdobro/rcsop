#include <gtest/gtest.h>
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Gt;
using ::testing::Return;

#include "utils/types.h"
#include "utils/mapping.h"
#include "colmap_observer_camera.h"

using rcsop::common::utils::points::vec3;

TEST(ColmapObserverCameraShould, MapToWorldToLocalBeEqual) {
    //TODO:
}