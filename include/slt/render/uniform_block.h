#ifndef SLT_RENDER_UNIFORM_BLOCK_H
#define SLT_RENDER_UNIFORM_BLOCK_H

#include "slt/render/details/backend_selection.h"

#ifdef SELECTED_SLT_RENDER_USE_OPENGL
#include "slt/render/details/opengl/uniform_block.h"
#endif

namespace slt {
namespace render {
using UniformBlockRef = _::ResourceRef<_::UniformBlock>;
}
}
#endif