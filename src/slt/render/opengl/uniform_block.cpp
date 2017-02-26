#include "slt/render/uniform_block.h"
#include "slt/log/log.h"
#include "slt/render/uniform_buffer.h"

namespace {
GLuint next_uniform_block_location = 0;
GLint max_uniform_block_bindings = 0;
std::vector<GLuint> bound_buffers;
}

namespace slt {
namespace render {
namespace _ {
UniformBlock::UniformBlock(std::size_t size) : size_(size) {
  if(next_uniform_block_location == 0) {
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_block_bindings);
    bound_buffers.resize(max_uniform_block_bindings);
  }
  if(next_uniform_block_location >= (GLuint)max_uniform_block_bindings) {
    // TODO: there is no way to alias/recycle uniform blocks at the moment.
    throw ResourceLoadError("Ran out of uniform blocks");
  }

  handle_ = next_uniform_block_location++;
}

UniformBlock::~UniformBlock() {}
}

UniformBufferUsage::UniformBufferUsage(UniformBufferRef const& ubo) {
  bound_cache_ = ubo.handle_cache_;
  SLT_ASSERT_EQ(0, bound_buffers[ubo.handle_cache_.bind_loc_]);
  glBindBufferBase(GL_UNIFORM_BUFFER, ubo.handle_cache_.bind_loc_,
                   ubo.handle_cache_.ubo_);
  bound_buffers[ubo.handle_cache_.bind_loc_] = ubo.handle_cache_.ubo_;
}

UniformBufferUsage::~UniformBufferUsage() {
  bound_buffers[bound_cache_.bind_loc_] = 0;
}
}
}