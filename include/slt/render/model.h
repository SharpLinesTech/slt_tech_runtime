#ifndef SLT_RENDER_MODEL_H
#define SLT_RENDER_MODEL_H

#include "slt/refl/refl.h"
#include "slt/render/details/backend_selection.h"

namespace slt {
namespace render {
enum class PrimType { TRIANGLES, TRI_STRIP };

struct ModelAttrib {
  uint8_t dimensions;
  uint16_t offset;
  std::string name;
};

struct ModelDrawOpData {
  PrimType prim;
  std::vector<uint16_t> indices;
};

struct ModelData {
  std::vector<float> data_;
  std::vector<ModelAttrib> attribs_;
  ModelDrawOpData drawOp_;
};
}
}

#ifdef SELECTED_SLT_RENDER_USE_OPENGL
#include "slt/render/details/opengl/model.h"
#endif

namespace slt {
namespace render {
using ModelRef = _::ResourceRef<_::Model>;

void draw(ModelRef const& model);
}

namespace refl {
REFL_STRUCT_TRAIT_BEGIN(::slt::render::ModelAttrib)
REFL_STRUCT_MEMBER(0, "dimensions", &::slt::render::ModelAttrib::dimensions)
REFL_STRUCT_MEMBER(1, "offset", &::slt::render::ModelAttrib::offset)
REFL_STRUCT_MEMBER(2, "name", &::slt::render::ModelAttrib::name)
REFL_STRUCT_TRAIT_END()

REFL_STRUCT_TRAIT_BEGIN(::slt::render::ModelDrawOpData)
REFL_STRUCT_MEMBER(0, "prim", &::slt::render::ModelDrawOpData::prim)
REFL_STRUCT_MEMBER(1, "indices", &::slt::render::ModelDrawOpData::indices)
REFL_STRUCT_TRAIT_END()

REFL_STRUCT_TRAIT_BEGIN(::slt::render::ModelData)
REFL_STRUCT_MEMBER(0, "data", &::slt::render::ModelData::data_)
REFL_STRUCT_MEMBER(1, "attribs", &::slt::render::ModelData::attribs_)
REFL_STRUCT_MEMBER(2, "ops", &::slt::render::ModelData::drawOp_)
REFL_STRUCT_TRAIT_END()

REFL_ENUM_TRAIT_BEGIN(::slt::render::PrimType)
REFL_ENUM_MEMBER(0, "TRIANGLES", ::slt::render::PrimType::TRIANGLES)
REFL_ENUM_MEMBER(1, "TRI_STRIP", ::slt::render::PrimType::TRI_STRIP)
REFL_ENUM_TRAIT_END()
}
}

#endif