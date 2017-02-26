#ifndef SLT_RENDER_SKELETON_H
#define SLT_RENDER_SKELETON_H

#include "slt/refl/refl.h"
#include "slt/render/details/backend_selection.h"

namespace slt {
namespace render {

struct Bone {
  int16_t parent_bone_index = -1;
  std::string name;
};

struct SkeletonData {
  std::vector<Bone> bones;
};
}
}

namespace slt {
namespace refl {
REFL_STRUCT_TRAIT_BEGIN(::slt::render::Bone)
REFL_STRUCT_MEMBER(0, "parent", &::slt::render::Bone::parent_bone_index)
REFL_STRUCT_MEMBER(1, "name", &::slt::render::Bone::name)
REFL_STRUCT_TRAIT_END()

REFL_STRUCT_TRAIT_BEGIN(::slt::render::SkeletonData)
REFL_STRUCT_MEMBER(0, "bones", &::slt::render::SkeletonData::bones)
REFL_STRUCT_TRAIT_END()
}
}

#endif