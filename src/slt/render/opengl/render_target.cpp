#include "slt/render/render_target.h"
#include "slt/render/details/opengl/gl_includes.h"

namespace slt {
namespace render {
namespace _ {
RenderTarget::RenderTarget(render::OffscreenRenderTargetConfig const& cfg) {
  glGenFramebuffers(1, &handle_);

  std::vector<GLenum> drawBuffers(cfg.colorTextures.size());

  glBindFramebuffer(GL_FRAMEBUFFER, handle_);

  for(int i = 0; i < cfg.colorTextures.size(); ++i) {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                         cfg.colorTextures[i].handle_cache_, 0);
    drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
  }

  if(cfg.depthTexture) {
    if(cfg.stencilTexture &&
       cfg.stencilTexture.handle_cache_ == cfg.depthTexture.handle_cache_) {
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           cfg.depthTexture.handle_cache_, 0);
    } else {
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           cfg.depthTexture.handle_cache_, 0);
    }
  }

  if(cfg.stencilTexture) {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                         cfg.stencilTexture.handle_cache_, 0);
  }

  glDrawBuffers(GLsizei(drawBuffers.size()), drawBuffers.data());

  auto fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(fb_status != GL_FRAMEBUFFER_COMPLETE) {
    glDeleteFramebuffers(1, &handle_);
    throw ResourceLoadError("Framgebuffer is not complete");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
  glDeleteFramebuffers(1, &handle_);
}
}
}
namespace {
bool bound_rdt = false;
}

namespace render {
RenderTargetUsage::RenderTargetUsage() {
  SLT_ASSERT(!bound_rdt);
  bound_rdt = true;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTargetUsage::RenderTargetUsage(RenderTargetRef const& r) {
  SLT_ASSERT(!bound_rdt);
  bound_rdt = true;
  glBindFramebuffer(GL_FRAMEBUFFER, r.handle_cache_);
}

RenderTargetUsage::~RenderTargetUsage() {
  bound_rdt = false;
}
}
}
