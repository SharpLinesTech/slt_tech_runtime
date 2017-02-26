#ifndef SLT_RENDER_OPENGL_UNIFORM_BLOCK_H
#define SLT_RENDER_OPENGL_UNIFORM_BLOCK_H

#include "slt/render/details/opengl/gl_includes.h"
#include "slt/render/details/resource.h"

namespace slt {
namespace render {
namespace _ {
class UniformBlock : public Resource<GLuint, UniformBlock> {
 public:
  UniformBlock(std::size_t size);
  ~UniformBlock();

  std::size_t size_;
};
}
}
}
#endif
