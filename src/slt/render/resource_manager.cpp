#include "slt/render/resource_manager.h"
#include "slt/core/core.h"
#include "slt/file/read.h"
#include "slt/log/log.h"
#include "slt/runtime/runtime.h"

namespace slt {

namespace render {
namespace {
const char* models_directory = "/models/";
const char* models_extension = ".mdl";

const char* programs_directory = "/render_programs/";
const char* programs_extension = ".prog";

template <typename RES_T, typename CACHE_T>
void performAsyncLoad(StringView path, StringView name, CACHE_T* cache,
                      ResourceManager* manager,
                      std::function<void(_::ResourceRef<RES_T>)> cb,
                      ResourceLoadFailedCallback fail_cb) {
  using DATA_T = typename RES_T::Data_t;

  auto perform_load = [name, cache, path, manager]() {
    manager->pending_loads_ += 1;
    auto name_str = name.toString();

    auto on_load = [name_str, cache, manager](DATA_T data) {
      try {
        cache->completePending(name_str, new RES_T(std::move(data), manager));
      } catch(std::exception const& e) {
        slt::log->info("resource initfailure: {} {}", name_str, e.what());
        cache->failPending(name_str, ResourceLoadError(e.what()));
      }

      manager->pending_loads_ -= 1;
    };

    auto on_fail = [name_str, cache, manager](auto err) {
      cache->failPending(name_str, ResourceLoadError(err.what()));
      slt::log->info("resource load failure: {}", name_str);
      manager->pending_loads_ -= 1;
    };

    file::asyncReadObject<DATA_T>(
        path.toString(), refl::SerializationFormat::RAW_UNVERIFIED_DATA,
        Core::instance->main_queue, on_load, on_fail);
  };

  cache->asyncAdd(name, cb, fail_cb, perform_load);
}
}

ModelRef ResourceManager::createModel(StringView name, ModelData const& data) {
  auto new_model = new _::Model(data, this);
  models_.add(name.toString(), new_model);
  return ModelRef(new_model);
}

void ResourceManager::loadModel(StringView name, ModelLoadCallback cb,
                                ResourceLoadFailedCallback fail_cb) {
  auto path = slt::assets_dir.get() + models_directory + name.toString() +
              models_extension;
  performAsyncLoad<_::Model>(path, name, &models_, this, cb, fail_cb);
}

void ResourceManager::loadProgram(StringView name, ProgramLoadCallback cb,
                                  ResourceLoadFailedCallback fail_cb) {
  auto path = slt::assets_dir.get() + programs_directory + name.toString() +
              programs_extension;
  performAsyncLoad<_::Program>(path, name, &programs_, this, cb, fail_cb);
}

TextureRef ResourceManager::createTexture(StringView name,
                                          TexConfig const& cfg) {
  auto new_tex = new _::Texture(cfg);
  textures_.add(name.toString(), new_tex);
  return TextureRef(new_tex);
}

UniformBufferRef ResourceManager::createUbo(StringView name,
                                            UniformBlockRef block) {
  auto new_ubo = new _::UniformBuffer(std::move(block));
  if(!name.empty()) {
    uniform_buffers_.add(name.toString(), new_ubo);
  }
  return UniformBufferRef(new_ubo);
}

RenderTargetRef ResourceManager::createRenderTarget(
    StringView name, OffscreenRenderTargetConfig const& cfg) {
  auto new_rdt = new _::RenderTarget(cfg);
  render_targets_.add(name.toString(), new_rdt);
  return RenderTargetRef(new_rdt);
}

UniformBlockRef ResourceManager::registerUniformBlock(StringView name,
                                                      std::size_t size) {
  auto new_ubb = new _::UniformBlock(size);
  uniform_blocks_.add(name.toString(), new_ubb);
  return UniformBlockRef(new_ubb);
}

UniformBlockRef ResourceManager::getUniformBlock(StringView name) {
  return uniform_blocks_.lookup(name);
}

void ResourceManager::registerAttribute(StringView name, uint16_t bind_loc) {
  attributes_[name.toString()] = bind_loc;
}

uint16_t ResourceManager::getAttributeBindLoc(std::string const& name) {
  auto bind_loc = attributes_.find(name);
  if(bind_loc == attributes_.end()) {
    throw ResourceLoadError("Unknown Attribute");
  }
  return bind_loc->second;
}

void ResourceManager::waitForAllPendingLoads() {
  Core::instance->main_queue.executeUntil([this](){
    return pending_loads_ == 0;
  });
}

void ResourceManager::loadDefaultResources() {
  registerAttribute("slt_vertex", 0);
  registerAttribute("slt_normal", 1);
  registerAttribute("slt_uv", 2);

  _::Model::Data_t unit_quad_data;

  unit_quad_data.attribs_.resize(1);
  unit_quad_data.attribs_[0].dimensions = 2;
  unit_quad_data.attribs_[0].name = "slt_vertex";
  unit_quad_data.attribs_[0].offset = 0;

  unit_quad_data.data_ = {
      0.0f, 0.0f, 
      0.0f, 1.0f, 
      1.0f, 0.0f, 
      1.0f, 1.0f,
  };

  unit_quad_data.drawOp_.prim = render::PrimType::TRI_STRIP;
  unit_quad_data.drawOp_.indices = {0, 1, 2, 3};

  auto unit_quad_model = new _::Model(unit_quad_data, this);
  unit_quad_ = ModelRef(unit_quad_model);

  models_.add("slt_unit_quad", unit_quad_model);
}
}
}