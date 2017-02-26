#include "slt/render/uniform_buffer.h"
#include "slt/log/log.h"

namespace slt {
namespace render {
namespace _ {

UniformBuffer::UniformBuffer(UniformBlockRef block) : block_(std::move(block)) {
  handle_.bind_loc_ = block_.handle_cache_;
  glGenBuffers(1, &handle_.ubo_);

  glBindBuffer(GL_UNIFORM_BUFFER, handle_.ubo_);
  glBufferData(GL_UNIFORM_BUFFER, block_.resource_->size_, nullptr,
               GL_STREAM_DRAW);
}

UniformBuffer::~UniformBuffer() {
  glDeleteBuffers(1, &handle_.ubo_);
}
}

void updateUboData(UniformBufferRef const& buf, DataView data) {
  glBindBuffer(GL_UNIFORM_BUFFER, buf.handle_cache_.ubo_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, data.size(), data.data());
}
}
}