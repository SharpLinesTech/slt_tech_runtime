#ifndef SLT_RENDER_OPENGL_UNIFORM_BUFFER_H
#define SLT_RENDER_OPENGL_UNIFORM_BUFFER_H

#include "slt/render/details/opengl/gl_includes.h"
#include "slt/render/details/resource.h"
#include "slt/render/uniform_block.h"

namespace slt {
namespace render {
namespace _ {

struct UniformBufferHandle {
  GLuint ubo_;
  GLuint bind_loc_;
};

class UniformBuffer : public Resource<UniformBufferHandle, UniformBuffer> {
 public:
  UniformBuffer(UniformBlockRef block);
  ~UniformBuffer();

  // Keep a hold on the block itself
  UniformBlockRef block_;
};
}
}
}
#endif
