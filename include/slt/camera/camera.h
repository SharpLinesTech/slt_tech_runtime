#ifndef SLT_CAMERA_CAMERA_H
#define SLT_CAMERA_CAMERA_H

#include "slt/debug/assert.h"
#include "slt/math/angle.h"
#include "slt/math/vector.h"
#include "slt/units/direction.h"
#include "slt/units/distance.h"
#include "slt/units/position.h"

namespace slt {
struct Camera {
  Pos3 pos = {0.0f, 0.0f, 0.0f};
  Dir3 forward = {0.0f, 0.0f, 1.0f};
  Dir3 up = {0.0f, 1.0f, 0.0f};
  Angle vfov = degrees(45.0f);
  Distance near_plane = 0.1f;
  Distance far_plane = 100.0f;

  Mat4x4 cameraToScreen(float aspect_ratio) const;
  Mat4x4 worldToCamera() const;
  Mat4x4 worldToScreen(float aspect_ratio) const;
};

inline void check(Camera const& cam) {
  SLT_ASSERT_FLOAT_EQ(dot(*cam.forward, *cam.up), 0.0f);
  SLT_ASSERT_GT(*cam.far_plane, *cam.near_plane);
  SLT_ASSERT_GT(cam.vfov, degrees(0.0f));
  SLT_ASSERT_LT(cam.vfov, degrees(180.0f));
}
}

#endif