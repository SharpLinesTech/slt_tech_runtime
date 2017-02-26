#ifndef SLT_RENDER_RESOURCE_MANAGER_H
#define SLT_RENDER_RESOURCE_MANAGER_H

#include <functional>

#include "slt/render/model.h"
#include "slt/render/program.h"
#include "slt/render/render_target.h"
#include "slt/render/texture.h"
#include "slt/render/uniform_block.h"
#include "slt/render/uniform_buffer.h"

namespace slt {
namespace render {

using ResourceLoadError = _::ResourceLoadError;

using ResourceLoadFailedCallback = std::function<void(ResourceLoadError)>;
using ModelLoadCallback = std::function<void(ModelRef)>;
using ProgramLoadCallback = std::function<void(ProgramRef)>;

class ResourceManager {
 public:
  // Resource creation:
  // Synchronous:
  RenderTargetRef createRenderTarget(StringView,
                                     OffscreenRenderTargetConfig const&);
  TextureRef createTexture(StringView, TexConfig const&);

  template <typename T>
  inline UniformBlockRef registerUniformBlock(StringView name);

  UniformBufferRef createUbo(StringView, UniformBlockRef);

  // Asynchronous:
  void loadModel(StringView, ModelLoadCallback,
                 ResourceLoadFailedCallback = nullptr);
  void loadProgram(StringView, ProgramLoadCallback,
                   ResourceLoadFailedCallback = nullptr);

  // Lookups
  UniformBlockRef getUniformBlock(StringView);

  // Secondary API.
  UniformBlockRef registerUniformBlock(StringView, std::size_t);
  void registerAttribute(StringView name, uint16_t bind_loc);
  uint16_t ResourceManager::getAttributeBindLoc(std::string const& name);

  void loadDefaultResources();

  ModelRef const& getUnitQuad() {
    return unit_quad_;
  }

 private:
  _::AsyncLoadingResourceCache<_::Program> programs_;
  _::AsyncLoadingResourceCache<_::Model> models_;

  _::ResourceCache<_::UniformBlock> uniform_blocks_;
  _::ResourceCache<_::UniformBuffer> uniform_buffers_;
  _::ResourceCache<_::RenderTarget> render_targets_;

  _::ResourceCache<_::Texture> textures_;

  std::map<std::string, uint16_t> attributes_;

  ModelRef unit_quad_;
};

template <typename T>
UniformBlockRef ResourceManager::registerUniformBlock(StringView name) {
  return registerUniformBlock(name, sizeof(T));
}
}
}

#endif