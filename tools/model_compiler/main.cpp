#include "slt/log/log.h"

#include <fbxsdk.h>
#include "fbx_memory_stream.h"

#include "slt/core/core.h"
#include "slt/file/read.h"
#include "slt/file/write.h"
#include "slt/mem/data_block.h"
#include "slt/render/model.h"
#include "slt/settings/settings.h"
#include "slt/units/direction.h"
#include "slt/units/position.h"

#include "mesh_builder.h"
#include "skeleton_builder.h"

#include <map>

slt::Setting<std::string> input_file("", "input",
                                     "the FBX file containing the input data");

slt::Setting<std::string> output_folder(
    "", "output", "The directory where to store the results");

FbxManager* fbx = nullptr;
std::map<std::string, slt::render::ModelData> result_models;

void explore(FbxNode* node, FbxScene* scene) {
  auto mesh = node->GetMesh();
  auto skeleton = node->GetSkeleton();

  fbxsdk::FbxNodeAttribute::EType node_attr_type =
      fbxsdk::FbxNodeAttribute::eUnknown;
  if(node->GetNodeAttribute()) {
    node_attr_type = node->GetNodeAttribute()->GetAttributeType();
  }

  auto node_transform = node->GetTransform();

  slt::log->info("Visiting: {}", node->GetName());

  if(mesh) {
    slt::log->info("found mesh: {}", node->GetName());
    slt::tools::MeshBuilder builder(mesh);

    result_models[node->GetName()] = std::move(builder.build());

  } else if(skeleton) {
    slt::log->info("found skeleton: {}", node->GetName());
    slt::tools::SkeletonBuilder builder(skeleton);
    std::move(builder.build());
  } else {
    for(int j = 0; j < node->GetChildCount(); j++) {
      explore(node->GetChild(j), scene);
    }
  }
}

FbxScene* loadScene(std::string const& path) {
  // Create an importer using the SDK manager.
  FbxImporter* lImporter = FbxImporter::Create(fbx, "");
  auto data = slt::file::read(path);
  sltFbxMemoryStream stream(fbx, data);

  // Use the first argument as the filename for the importer.
  if(!lImporter->Initialize(&stream, data.data(), -1, fbx->GetIOSettings())) {
    slt::log->error("Failed to initialize FBX importer: {}",
                    lImporter->GetStatus().GetErrorString());
    std::terminate();
  }

  FbxScene* lScene = FbxScene::Create(fbx, "myScene");
  lImporter->Import(lScene);
  lImporter->Destroy();

  FbxGeometryConverter converter(fbx);
  converter.Triangulate(lScene, true);

  auto scale_factor =
      lScene->GetGlobalSettings().GetSystemUnit().GetScaleFactor();
  (void)scale_factor;
  return lScene;
}

int main(int argc, const char* argv[]) {
  slt::Core core(argc, argv);

  fbx = FbxManager::Create();
  fbx->SetLocale("UTF-8");

  FbxIOSettings* ios = FbxIOSettings::Create(fbx, IOSROOT);
  fbx->SetIOSettings(ios);

  auto scene = loadScene(input_file.get());
  explore(scene->GetRootNode(), scene);

  for(auto const& m : result_models) {
    std::string dst = output_folder.get() + "/" + m.first + ".mdl";
    auto data = slt::refl::serialize(
        m.second, slt::refl::SerializationFormat::RAW_UNVERIFIED_DATA);

    slt::file::write(data, dst);
  }
  return 0;
}