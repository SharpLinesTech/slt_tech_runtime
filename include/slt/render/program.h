#ifndef SLT_RENDER_PROGRAM_H
#define SLT_RENDER_PROGRAM_H

#include "slt/refl/refl.h"
#include "slt/render/details/backend_selection.h"

namespace slt {
namespace render {

struct ProgramData {
  std::string vertexShader;
  std::string geometryShader;
  std::string fragmentShader;
};
}

namespace refl {
// PROGRAM
REFL_STRUCT_TRAIT_BEGIN(::slt::render::ProgramData)
REFL_STRUCT_MEMBER(0, "vertexShader", &::slt::render::ProgramData::vertexShader)
REFL_STRUCT_MEMBER(1, "geometryShader",
                   &::slt::render::ProgramData::geometryShader)
REFL_STRUCT_MEMBER(2, "fragmentShader",
                   &::slt::render::ProgramData::fragmentShader)
REFL_STRUCT_TRAIT_END()
}
}

#ifdef SELECTED_SLT_RENDER_USE_OPENGL
#include "slt/render/details/opengl/program.h"
#endif

namespace slt {
namespace render {
using ProgramRef = _::ResourceRef<_::Program>;

struct ProgramUsage {
  ProgramUsage(ProgramRef const& prg);
  ~ProgramUsage();
};
}
}

#endif