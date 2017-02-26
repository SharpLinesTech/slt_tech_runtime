#ifndef SLT_RENDER_OPENGL_PROGRAM_H
#define SLT_RENDER_OPENGL_PROGRAM_H

#include "slt/refl/refl.h"
#include "slt/render/details/opengl/gl_includes.h"
#include "slt/render/details/resource.h"

namespace slt {
namespace render {
namespace _ {

struct Program : public Resource<GLuint, Program> {
  using Data_t = render::ProgramData;

  Program(Data_t const&, ResourceManager*);
  ~Program();

  void assignCache(AsyncLoadingResourceCache<Program>* cache) {
    cache_ = cache;
  }

 private:
  AsyncLoadingResourceCache<Program>* cache_ = nullptr;

  void link_();
  void resolveAttributes_(ResourceManager*);
  void resolveUniformBuffers_(ResourceManager*);
};
}
}
}

#endif