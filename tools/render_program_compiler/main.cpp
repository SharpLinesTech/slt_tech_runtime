#include "slt/core/core.h"
#include "slt/file/read.h"
#include "slt/file/write.h"
#include "slt/log/log.h"
#include "slt/mem/data_block.h"
#include "slt/refl/refl.h"
#include "slt/render/program.h"
#include "slt/settings/settings.h"

#include <map>

enum class ShaderType { VERTEX, FRAGMENT, GEOMETRY };

struct ShaderConfig {
  ShaderType type;
  std::vector<std::string> sources;
};

struct ProgramConfig {
  std::string description;
  std::vector<ShaderConfig> shaders;
};

namespace slt {
namespace refl {
REFL_STRUCT_TRAIT_BEGIN(::ShaderConfig)
REFL_STRUCT_MEMBER(0, "type", &::ShaderConfig::type)
REFL_STRUCT_MEMBER(1, "sources", &::ShaderConfig::sources)
REFL_STRUCT_TRAIT_END()

REFL_STRUCT_TRAIT_BEGIN(::ProgramConfig)
REFL_STRUCT_MEMBER(0, "description", &::ProgramConfig::description)
REFL_STRUCT_MEMBER(1, "shaders", &::ProgramConfig::shaders)
REFL_STRUCT_TRAIT_END()

REFL_ENUM_TRAIT_BEGIN(::ShaderType)
REFL_ENUM_MEMBER(0, "VERTEX", ShaderType::VERTEX)
REFL_ENUM_MEMBER(1, "FRAGMENT", ShaderType::FRAGMENT)
REFL_ENUM_MEMBER(2, "GEOMETRY", ShaderType::GEOMETRY)
REFL_ENUM_TRAIT_END()
}
}

slt::Setting<std::string> programConfig("", "input", "Program config file");

slt::Setting<std::string> outputFile(
    "", "output", "File where to store the resulting shader.");

int main(int argc, const char* argv[]) {
  slt::Core core(argc, argv);

  try {
    auto cfg = slt::file::readObject<ProgramConfig>(
        programConfig.get(), slt::refl::SerializationFormat::HUMAN_READABLE);

    slt::render::ProgramData result;

    for(auto const& shader : cfg.shaders) {
      std::string code;
      for(auto const& src : shader.sources) {
        auto data = slt::file::read(src);

        code += std::string(begin(data), end(data));
        code += '\n';
      }

      switch(shader.type) {
        case ShaderType::VERTEX:
          result.vertexShader = code;
          break;
        case ShaderType::FRAGMENT:
          result.fragmentShader = code;
          break;
        case ShaderType::GEOMETRY:
          result.geometryShader = code;
          break;
      }
    }

    auto data = slt::refl::serialize(
        result, slt::refl::SerializationFormat::RAW_UNVERIFIED_DATA);

    slt::file::write(data, outputFile.get());
  } catch(std::exception const& e) {
    slt::log->error("Uncaught exception: {}", e.what());
    return 1;
  }
  return 0;
}