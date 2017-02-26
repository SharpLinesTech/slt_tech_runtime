#ifndef SLT_TOOLS_MODEL_COMPILER_SKELETON_BUILDER_H
#define SLT_TOOLS_MODEL_COMPILER_SKELETON_BUILDER_H

#include <fbxsdk.h>

#include "slt/render/skeleton.h"

namespace slt {
namespace tools {
class SkeletonBuilder {
 public:
  SkeletonBuilder(FbxSkeleton* mesh);
  slt::render::SkeletonData build();

 private:
  void handle(FbxNode* bone_node, slt::render::SkeletonData& dst,
              int parent_index);
  FbxSkeleton* skel_;
};
}
}

#endif