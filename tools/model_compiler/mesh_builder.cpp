#include "slt/log/log.h"

#include "mesh_builder.h"

slt::Setting<std::string> vertex_attrib_name(
    "slt_vertex", "vertex_attrib",
    "The attribute name to assign tp vertices when exporting a mesh.");

slt::Setting<std::string> normal_attrib_name("slt_normal", "normal_attrib",
                                             "The attribute name to give "
                                             "normals when exporting a mesh "
                                             "(set to empty to not export "
                                             "normals).");

slt::Setting<std::string> uv_attrib_name("slt_uv", "uv_attrib",
                                         "The attribute name to give "
                                         "UVs when exporting a mesh "
                                         "(set to empty to not export "
                                         "normals).");

namespace {
template <typename LAYER_ELEM_T>
size_t resolveAttribIndex(FbxMesh* mesh, LAYER_ELEM_T& layer_elements,
                          unsigned int vertex_index, int poly_id, int v_id) {
  size_t result = 0;

  auto mapping_mode = layer_elements.GetMappingMode();
  auto ref_mode = layer_elements.GetReferenceMode();

  switch(mapping_mode) {
    case FbxGeometryElement::eByControlPoint:
      result = mesh->GetPolygonVertex(poly_id, v_id);
      break;
    case FbxGeometryElement::eByPolygonVertex:
      result = vertex_index;
      break;
    default:
      slt::log->error("Unhandled attrib mapping mode");
      std::terminate();
      break;
  }

  switch(ref_mode) {
    case FbxGeometryElement::eDirect:
      break;
    case FbxGeometryElement::eIndexToDirect:
      result = layer_elements.GetIndexArray().GetAt(int(result));
      break;
    default:
      slt::log->error("Unhandled attrib ref mode");
      std::terminate();
      break;
  }

  return result;
}

template <typename LAYER_ELEM_T>
slt::tools::MeshBuilder::SingleAttriData extractAttrib(
    FbxMesh* mesh, std::string const& name, LAYER_ELEM_T& layer_elements,
    unsigned int dims) {
  unsigned int vertex_id = 0;

  slt::tools::MeshBuilder::SingleAttriData result;
  result.name = name;
  result.dim = dims;

  auto polys = mesh->GetPolygonCount();
  for(auto p = 0; p < polys; p++) {
    int p_size = mesh->GetPolygonSize(p);
    SLT_ASSERT_EQ(p_size, 3);

    for(int point = 0; point < p_size; ++point) {
      std::size_t attr_index =
          resolveAttribIndex(mesh, layer_elements, vertex_id, p, point);

      auto attrib_data = layer_elements.GetDirectArray().GetAt(int(attr_index));
      for(unsigned int i = 0; i < dims; ++i) {
        result.data.emplace_back(float(attrib_data[i]));
      }

      ++vertex_id;
    }
  }
  return result;
}

slt::render::ModelData compileModel(
    std::vector<slt::tools::MeshBuilder::SingleAttriData> data) {
  std::vector<uint16_t> tri_indices;

  auto raw_vertex_count = data[0].data.size() / data[0].dim;
  SLT_ASSERT_EQ(raw_vertex_count % 3, 0);  // Aren't we building triangles?

  for(auto const& raw_attrib : data) {
    SLT_ASSERT_EQ(raw_attrib.data.size(), raw_vertex_count * raw_attrib.dim);
  }

  std::vector<float> final_data;
  std::map<std::vector<float>, unsigned int> reverse_vertex_map;

  for(std::size_t i = 0; i < raw_vertex_count; ++i) {
    std::vector<float> vertex;
    for(auto const& raw_attrib : data) {
      auto data_start = i * raw_attrib.dim;
      auto data_end = (i + 1) * raw_attrib.dim;
      vertex.insert(vertex.end(), raw_attrib.data.begin() + data_start,
                    raw_attrib.data.begin() + data_end);
    }

    auto found = reverse_vertex_map.find(vertex);

    if(found == reverse_vertex_map.end()) {
      unsigned int new_vertex_index = unsigned int(reverse_vertex_map.size());

      final_data.insert(final_data.end(), vertex.begin(), vertex.end());
      reverse_vertex_map.emplace(vertex, new_vertex_index);
      tri_indices.emplace_back(new_vertex_index);
    } else {
      tri_indices.emplace_back(found->second);
    }
  }
  slt::render::ModelData result;
  result.drawOp_.prim = slt::render::PrimType::TRIANGLES;
  result.drawOp_.indices = std::move(tri_indices);

  result.data_ = std::move(final_data);

  uint16_t offset = 0;
  for(auto const& raw_attrib : data) {
    slt::render::ModelAttrib built_attrib;
    built_attrib.name = raw_attrib.name;
    built_attrib.dimensions = raw_attrib.dim;
    built_attrib.offset = offset;
    offset += raw_attrib.dim;

    result.attribs_.emplace_back(built_attrib);
  }

  return result;
}
}

namespace slt {
namespace tools {
MeshBuilder::MeshBuilder(FbxMesh* mesh) : mesh_(mesh) {}

MeshBuilder::SingleAttriData MeshBuilder::getVertices() {
  SingleAttriData result;

  result.dim = 3;
  result.name = vertex_attrib_name.get();
  slt::log->info("using vertex attrib name: {}", vertex_attrib_name.get());
  auto points = mesh_->GetControlPoints();

  float scale = 0.01f;
  scale *= (float)mesh_->GetScene()
               ->GetGlobalSettings()
               .GetSystemUnit()
               .GetScaleFactor();

  auto polys = mesh_->GetPolygonCount();
  unsigned int vertex_id = 0;
  for(auto poly = 0; poly < polys; poly++) {
    int p_size = mesh_->GetPolygonSize(poly);

    if(p_size != 3) {
      slt::log->error("Non triangle mesh: {}", mesh_->GetNode()->GetName());
      std::terminate();
    }

    for(int point = 0; point < p_size; ++point) {
      auto vertex = mesh_->GetPolygonVertex(poly, point);

      result.data.emplace_back((float)points[vertex][0] * scale);
      result.data.emplace_back((float)points[vertex][1] * scale);
      result.data.emplace_back((float)points[vertex][2] * scale);
    }
  }
  return result;
}

slt::render::ModelData MeshBuilder::build() {
  std::vector<SingleAttriData> raw_data;

  // We always have vertices.
  raw_data.emplace_back(getVertices());

  int normal_set_found = 0;
  int uv_sets_found = 0;

  auto num_layers = mesh_->GetLayerCount();
  for(auto layer_index = 0; layer_index < num_layers; ++layer_index) {
    auto layer = mesh_->GetLayer(layer_index);
    slt::log->info("  Layer {}:", layer_index);

    auto normals = layer->GetNormals();
    auto uv_sets = layer->GetUVSets();
    // auto binormals = layer->GetBinormals();
     //auto tangents = layer->GetTangents();
     //auto v_color = layer->GetVertexColors();

    if(normals && !normal_attrib_name.get().empty()) {
      if(normal_set_found != 0) {
        slt::log->error(
            "    Multiple normal sets, not sure what do do with them.");
        std::terminate();
      }

      raw_data.emplace_back(
          extractAttrib(mesh_, normal_attrib_name.get(), *normals, 3));
      ++normal_set_found;
    }

    for(auto uv_id = 0; uv_id < uv_sets.Size(); ++uv_id) {
      if(uv_sets_found != 0 && !uv_attrib_name.get().empty()) {
        slt::log->error("    Multiple UV sets, not sure what do do with them.");
        std::terminate();
      }

      raw_data.emplace_back(
          extractAttrib(mesh_, uv_attrib_name.get(), *uv_sets[uv_id], 2));
      ++uv_sets_found;
    }
  }
  return compileModel(std::move(raw_data));
}
}
}