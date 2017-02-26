#ifndef SLT_TOOLS_MODEL_COMPILER_MESH_BUILDER_H
#define SLT_TOOLS_MODEL_COMPILER_MESH_BUILDER_H

#include <fbxsdk.h>

#include "slt/render/model.h"

namespace slt {
namespace tools {
class MeshBuilder {
 public:
  MeshBuilder(FbxMesh* mesh);

  void explore();

  slt::render::ModelData build();

  struct SingleAttriData {
    std::string name;
    unsigned int dim;
    std::vector<float> data;
  };

 private:
  SingleAttriData getVertices();
  FbxMesh* mesh_;
};
}
}

#endif