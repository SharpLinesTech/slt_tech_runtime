SET(SLT_CAMERA_PRIVATE
  camera.cpp
)

SET(SLT_CAMERA_PUBLIC
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/camera/camera.h
)


add_library(slt_cam OBJECT
  ${SLT_CAMERA_PRIVATE} ${SLT_CAMERA_PUBLIC}
)


SLT_MODULE_CONFIG(slt_cam)