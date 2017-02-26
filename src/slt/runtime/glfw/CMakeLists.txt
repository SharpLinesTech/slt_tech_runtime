SET(SLT_RUNTIME_GLFW_PRIVATE
  controller.cpp
  glfw_runtime.cpp
)

SET(SLT_RUNTIME_GLFW_PUBLIC
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/runtime/details/glfw/controller.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/runtime/details/glfw/glfw_runtime.h
)

add_library(slt_runtime_glfw_impl OBJECT
  ${SLT_RUNTIME_GLFW_PRIVATE}
  ${SLT_RUNTIME_GLFW_PUBLIC}
)

SLT_MODULE_CONFIG(slt_runtime_glfw_impl)
