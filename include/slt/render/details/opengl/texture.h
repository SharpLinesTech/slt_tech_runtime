#ifndef SLT_RENDER_OPENGL_TEXTURE_H
#define SLT_RENDER_OPENGL_TEXTURE_H

#include "slt/render/details/opengl/gl_includes.h"
#include "slt/render/details/resource.h"
#include "slt/render/texture.h"

namespace slt {
namespace render {
namespace _ {

struct Texture : public Resource<GLuint, Texture> {
  Texture(TexConfig const& cfg);
  ~Texture();
};
}
}
}

#endif
