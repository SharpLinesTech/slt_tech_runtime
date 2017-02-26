#ifndef SLT_RENDER_TEXTURE_H
#define SLT_RENDER_TEXTURE_H

#include "slt/render/details/backend_selection.h"

namespace slt {
namespace render {

// Format of a texture as encoded on the GPU.
enum class TexFormat { RGBA_8, DEPTH };

// Format of texture data when uploading.
enum class TexDataFormat { RGBA_8, BGRA_8 };

struct TexConfig {
  unsigned int width;
  unsigned int height;
  TexFormat tex_format = TexFormat::RGBA_8;
  unsigned int levels = 1;
};
}
}

#ifdef SELECTED_SLT_RENDER_USE_OPENGL
#include "slt/render/details/opengl/texture.h"
#endif

namespace slt {
namespace render {
using TextureRef = _::ResourceRef<_::Texture>;

using TexBinding = std::pair<TextureRef const&, uint16_t>;
struct TextureUsage {
  TextureUsage(TexBinding);
  TextureUsage(TexBinding, TexBinding);
  TextureUsage(TexBinding, TexBinding, TexBinding);
  ~TextureUsage();
};
}
}
#endif