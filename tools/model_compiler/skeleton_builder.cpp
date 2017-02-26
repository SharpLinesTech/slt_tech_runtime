#include "slt/log/log.h"

#include "skeleton_builder.h"

namespace slt {
namespace tools {
SkeletonBuilder::SkeletonBuilder(FbxSkeleton* skel) : skel_(skel) {}

slt::render::SkeletonData SkeletonBuilder::build() {
  slt::render::SkeletonData result;

  handle(skel_->GetNode(), result, -1);

  return result;
}

void SkeletonBuilder::handle(FbxNode* bone_node, slt::render::SkeletonData& dst,
                             int parent_index) {
  int index = (int)dst.bones.size();

  slt::log->info("BONE!!!!! {}", bone_node->GetName());
  dst.bones.emplace_back();
  dst.bones.back().parent_bone_index = parent_index;
  dst.bones.back().name = bone_node->GetName();

  for(int i = 0; i < bone_node->GetChildCount(); i++) {
    auto node = bone_node->GetChild(i);
    if(node &&
       node->GetNodeAttribute()->GetAttributeType() ==
           FbxNodeAttribute::eSkeleton) {
      handle(node, dst, index);
    }
  }
}
}
}