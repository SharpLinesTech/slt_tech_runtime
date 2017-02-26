#include "slt/render/model.h"

#include "slt/core/core.h"
#include "slt/file/read.h"
#include "slt/log/log.h"
#include "slt/runtime/runtime.h"

#include <set>

namespace {
const char* models_directory = "/models/";
const char* programs_extension = ".mdl";

GLenum GetGLPrimType(slt::render::PrimType type) {
  switch(type) {
    case slt::render::PrimType::TRIANGLES:
      return GL_TRIANGLES;
    case slt::render::PrimType::TRI_STRIP:
      return GL_TRIANGLE_STRIP;
  }

  slt::log->error("unknown primitive type");
  return GL_TRIANGLES;
}
}

namespace slt {
namespace render {
namespace _ {
Model::Model(render::ModelData const& data, render::ResourceManager* res_man) {
  glGenBuffers(1, &vertices_vbo_);
  glGenBuffers(1, &indices_vbo_);

  glGenVertexArrays(1, &handle_.vao_);
  glBindVertexArray(handle_.vao_);

  glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_);

  // Send the vertex data to the GPU
  glBufferData(GL_ARRAY_BUFFER, data.data_.size() * sizeof(data.data_[0]),
               data.data_.data(), GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               data.drawOp_.indices.size() * sizeof(data.drawOp_.indices[0]),
               data.drawOp_.indices.data(), GL_STATIC_DRAW);

  GLsizei stride = 0;

  for(auto const& attr : data.attribs_) {
    stride += attr.dimensions * sizeof(float);
  }

  for(auto const& attr : data.attribs_) {
    auto index = res_man->getAttributeBindLoc(attr.name);

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, attr.dimensions, GL_FLOAT, false, stride,
                          (void*)(attr.offset * sizeof(float)));
  }

  glBindVertexArray(0);

  handle_.primitive = GetGLPrimType(data.drawOp_.prim);
  handle_.vertices_count_ = unsigned int(data.drawOp_.indices.size());
}

Model::~Model() {}
}

void draw(ModelRef const& model) {
  glBindVertexArray(model.handle_cache_.vao_);
  glDrawElements(model.handle_cache_.primitive,
                 model.handle_cache_.vertices_count_, GL_UNSIGNED_SHORT, 0);
}
}
}