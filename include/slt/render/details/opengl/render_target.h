#ifndef SLT_RENDER_OPENGL_RENDER_TARGET_H
#define SLT_RENDER_OPENGL_RENDER_TARGET_H

#include "slt/render/details/opengl/gl_includes.h"
#include "slt/render/details/resource.h"

namespace slt {
namespace render {
namespace _ {

struct RenderTarget : public Resource<GLuint, RenderTarget> {
  RenderTarget(OffscreenRenderTargetConfig const& cfg);
  ~RenderTarget();

 private:
  GLuint vertices_vbo_;
  GLuint indices_vbo_;
};
}
}
}

#endif