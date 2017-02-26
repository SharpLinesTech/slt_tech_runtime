#ifndef SLT_RENDER_RENDERTARGET_H
#define SLT_RENDER_RENDERTARGET_H

#include "slt/render/details/backend_selection.h"
#include "slt/render/texture.h"

namespace slt {
namespace render {

struct MainRenderTargetConfig {
  uint16_t width;
  uint16_t height;
};

struct OffscreenRenderTargetConfig {
  std::vector<TextureRef> colorTextures;
  TextureRef depthTexture;
  TextureRef stencilTexture;
};
}
}

#ifdef SELECTED_SLT_RENDER_USE_OPENGL
#include "slt/render/details/opengl/render_target.h"
#endif

namespace slt {
namespace render {
using RenderTargetRef = _::ResourceRef<_::RenderTarget>;

struct RenderTargetUsage {
  RenderTargetUsage();
  RenderTargetUsage(RenderTargetRef const &);
  ~RenderTargetUsage();
};
}
}
#endif