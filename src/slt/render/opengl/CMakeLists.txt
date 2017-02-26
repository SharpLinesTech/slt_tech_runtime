SET(SLT_RENDER_GL_PRIVATE
  model.cpp
  program.cpp
  render_target.cpp
  texture.cpp
  uniform_block.cpp
  uniform_buffer.cpp
)

SET(SLT_RENDER_GL_PUBLIC
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/details/opengl/gl_includes.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/details/opengl/model.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/details/opengl/program.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/details/opengl/render_target.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/details/opengl/uniform_block.h
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/render/details/opengl/uniform_buffer.h
)

add_library(slt_render_opengl OBJECT
  ${SLT_RENDER_GL_PRIVATE}
  ${SLT_RENDER_GL_PUBLIC}
)

SLT_MODULE_CONFIG(slt_render_opengl)
