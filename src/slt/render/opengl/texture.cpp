#include "slt/render/texture.h"
#include "slt/render/details/opengl/texture.h"

namespace {
GLenum convertTexFormat(slt::render::TexFormat fmt) {
  switch(fmt) {
    case slt::render::TexFormat::RGBA_8:
      return GL_RGBA8;
    case slt::render::TexFormat::DEPTH:
      return GL_DEPTH_COMPONENT32F;
    default:
      break;
  }

  return GL_RGBA8;
}

// (format/type)
std::pair<GLenum, GLenum> convertTexDataFormat(slt::render::TexDataFormat fmt) {
  switch(fmt) {
    case slt::render::TexDataFormat::RGBA_8:
      return std::make_pair(GL_RGBA, GL_UNSIGNED_BYTE);
    case slt::render::TexDataFormat::BGRA_8:
      return std::make_pair(GL_BGRA, GL_UNSIGNED_BYTE);
    default:
      break;
  }

  return std::make_pair(GL_RGBA, GL_UNSIGNED_BYTE);
}

// Returns the expected byte size for a single piexel
std::size_t expectedSize(slt::render::TexDataFormat fmt, unsigned int w,
                         unsigned int h) {
  std::size_t result = 1;
  switch(fmt) {
    case slt::render::TexDataFormat::RGBA_8:
    case slt::render::TexDataFormat::BGRA_8:
      result = 4 * w * h;
      break;
    default:
      break;
  }
  return result;
}
}

namespace slt {
namespace render {

TextureUsage::TextureUsage(TexBinding b0) {
  glActiveTexture(GL_TEXTURE0 + b0.second);
  glBindTexture(GL_TEXTURE_2D, b0.first.handle_cache_);
}

TextureUsage::TextureUsage(TexBinding b0, TexBinding b1) {
  glActiveTexture(GL_TEXTURE0 + b0.second);
  glBindTexture(GL_TEXTURE_2D, b0.first.handle_cache_);

  glActiveTexture(GL_TEXTURE0 + b1.second);
  glBindTexture(GL_TEXTURE_2D, b1.first.handle_cache_);
}

TextureUsage::TextureUsage(TexBinding b0, TexBinding b1, TexBinding b2) {
  glActiveTexture(GL_TEXTURE0 + b0.second);
  glBindTexture(GL_TEXTURE_2D, b0.first.handle_cache_);

  glActiveTexture(GL_TEXTURE0 + b1.second);
  glBindTexture(GL_TEXTURE_2D, b1.first.handle_cache_);

  glActiveTexture(GL_TEXTURE0 + b2.second);
  glBindTexture(GL_TEXTURE_2D, b2.first.handle_cache_);
}

TextureUsage::~TextureUsage() {}

namespace _ {
Texture::Texture(render::TexConfig const& cfg) {
  glGenTextures(1, &handle_);

  glBindTexture(GL_TEXTURE_2D, handle_);
  glTexStorage2D(GL_TEXTURE_2D, cfg.levels, convertTexFormat(cfg.tex_format),
                 cfg.width, cfg.height);
}

Texture::~Texture() {
  glDeleteTextures(1, &handle_);
}
}

void updateTexture(TextureRef const& tex, uint32_t x, uint32_t y, uint32_t w,
                   uint32_t h, DataView data, TexDataFormat format) {
  SLT_ASSERT_EQ(expectedSize(format, w, h), data.size());
  glBindTexture(GL_TEXTURE_2D, tex.handle_cache_);
  auto fmt = convertTexDataFormat(format);
  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, fmt.first, fmt.second,
                  data.data());
}
}
}