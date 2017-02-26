SET(SLT_RUNTIME_COMMON_PRIVATE
  runtime.cpp
)

SET(SLT_RUNTIME_COMMON_PUBLIC
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/runtime/runtime.h
)

add_library(slt_runtime_common OBJECT
  ${SLT_RUNTIME_COMMON_PRIVATE}
  ${SLT_RUNTIME_COMMON_PUBLIC}
)

SLT_MODULE_CONFIG(slt_runtime_common)

add_subdirectory(glfw)