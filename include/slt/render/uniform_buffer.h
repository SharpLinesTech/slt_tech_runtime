#ifndef SLT_RENDER_UNIFORM_BUFFER_H
#define SLT_RENDER_UNIFORM_BUFFER_H

#include "slt/render/details/backend_selection.h"

#ifdef SELECTED_SLT_RENDER_USE_OPENGL
#include "slt/render/details/opengl/uniform_buffer.h"
#endif

namespace slt {
namespace render {
using UniformBufferRef = _::ResourceRef<_::UniformBuffer>;

void updateUboData(UniformBufferRef const&, DataView);

struct UniformBufferUsage {
  UniformBufferUsage(UniformBufferRef const& prg);
  ~UniformBufferUsage();

  UniformBufferRef::handle_t bound_cache_;
};
}
}
#endif