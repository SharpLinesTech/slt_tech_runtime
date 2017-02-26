#include "slt/camera/camera.h"

namespace slt {

Mat4x4 Camera::cameraToScreen(float aspect_ratio) const {
  return perspectiveTransform(vfov, aspect_ratio, near_plane, far_plane);
}

Mat4x4 Camera::worldToCamera() const {
  return glm::lookAt(*pos, *pos + *forward, *up);
  return Mat4x4();
}

Mat4x4 Camera::worldToScreen(float aspect_ratio) const {
  return cameraToScreen(aspect_ratio) * worldToCamera();
}
}