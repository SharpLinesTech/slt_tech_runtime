
#ifndef SLT_RENDER_OPENGL_MODEL_H
#define SLT_RENDER_OPENGL_MODEL_H

#include "slt/render/details/opengl/gl_includes.h"
#include "slt/render/details/resource.h"

namespace slt {
namespace render {
namespace _ {

struct ModelHandle {
  GLuint vao_;
  uint32_t vertices_count_;
  GLenum primitive;
};

struct Model : public Resource<ModelHandle, Model> {
  using Data_t = slt::render::ModelData;

  Model(Data_t const&, ResourceManager*);
  ~Model();

 private:
  GLuint vertices_vbo_;
  GLuint indices_vbo_;
};
}
}
}
#endif