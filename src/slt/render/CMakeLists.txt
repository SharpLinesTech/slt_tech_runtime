add_subdirectory(opengl)

SET(SLT_RENDER_PRIVATE
  resource_manager.cpp
)

SET(SLT_RENDER_PUBLIC
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/resource_manager.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/model.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/skeleton.h
)

add_library(slt_render OBJECT
  ${SLT_RENDER_PRIVATE}
  ${SLT_RENDER_PUBLIC}
)

SLT_MODULE_CONFIG(slt_render)
