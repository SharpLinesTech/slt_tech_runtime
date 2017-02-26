#include "slt/render/program.h"
#include "slt/core/core.h"
#include "slt/file/read.h"
#include "slt/log/log.h"
#include "slt/render/resource_manager.h"
#include "slt/render/uniform_block.h"
#include "slt/runtime/runtime.h"

#include <set>

namespace {

const char* programs_directory = "/render_programs/";
const char* programs_extension = ".prog";

GLuint createShader(GLenum type, slt::StringView code) {
  GLuint result = glCreateShader(type);

  GLsizei length = GLsizei(code.length());

  const char* src = code.data();
  glShaderSource(result, 1, &src, &length);
  glCompileShader(result);
  GLint isCompiled = 0;
  glGetShaderiv(result, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE) {
    GLint info_length = 0;
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &info_length);

    std::vector<GLchar> errlog(info_length);
    glGetShaderInfoLog(result, info_length, &info_length, &errlog[0]);
    std::string err_str(errlog.begin(), errlog.end());
    slt::log->error("shader compilation failure: {}", err_str);
    glDeleteShader(result);

    throw slt::render::_::ResourceLoadError(err_str);
  }

  return result;
}
}

namespace slt {
namespace render {
namespace _ {
AsyncLoadingResourceCache<Program> programs_cache;

Program::Program(ProgramData const& data, ResourceManager* manager) {
  GLuint vshader = 0;
  GLuint fshader = 0;
  GLuint gShader = 0;

  try {
    vshader = createShader(GL_VERTEX_SHADER, data.vertexShader);
    fshader = createShader(GL_FRAGMENT_SHADER, data.fragmentShader);
    if(!data.geometryShader.empty()) {
      gShader = createShader(GL_GEOMETRY_SHADER, data.geometryShader);
    }
  } catch(std::exception&) {
    if(vshader) {
      glDeleteShader(vshader);
    }
    if(fshader) {
      glDeleteShader(vshader);
    }
    if(gShader) {
      glDeleteShader(vshader);
    }
    throw;
  }

  handle_ = glCreateProgram();
  glAttachShader(handle_, vshader);
  glDeleteShader(vshader);
  glAttachShader(handle_, fshader);
  glDeleteShader(fshader);
  if(gShader) {
    glAttachShader(handle_, gShader);
    glDeleteShader(gShader);
  }

  try {
    link_();
    resolveAttributes_(manager);
    link_();  // We need to relink for the attribute bindings to take effect.
    resolveUniformBuffers_(manager);
  } catch(std::exception&) {
    glDeleteProgram(handle_);
    throw;
  }

  glDetachShader(handle_, vshader);
  glDetachShader(handle_, fshader);
  if(gShader) {
    glDetachShader(handle_, gShader);
  }
}

void Program::link_() {
  glLinkProgram(handle_);

  GLint isLinked = 0;
  glGetProgramiv(handle_, GL_LINK_STATUS, (int*)&isLinked);
  if(isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(handle_, GL_INFO_LOG_LENGTH, &maxLength);
    // The maxLength includes the NULL character
    std::vector<GLchar> errlog(maxLength);
    glGetProgramInfoLog(handle_, maxLength, &maxLength, &errlog[0]);
    std::string err_str(errlog.begin(), errlog.end());
    slt::log->error("shader compilation failure: {}", err_str);

    throw ResourceLoadError(err_str);
  }
}

void Program::resolveAttributes_(ResourceManager* manager) {
  GLint attribs_count = 0;
  glGetProgramiv(handle_, GL_ACTIVE_ATTRIBUTES, &attribs_count);

  std::set<unsigned int> used_attrib_locations;

  for(int i = 0; i < attribs_count; ++i) {
    const int max_name_length = 128;
    char name[max_name_length];
    GLsizei name_length = 0;
    GLint size = 0;
    GLenum type;
    glGetActiveAttrib(handle_, i, max_name_length, &name_length, &size, &type,
                      name);

    try {
      auto bind_loc = manager->getAttributeBindLoc(name);

      if(used_attrib_locations.find(bind_loc) != used_attrib_locations.end()) {
        slt::log->error(
            "Render program uses the same attib location more than once");
        throw ResourceLoadError("invalid atttrib setup");
      }
      used_attrib_locations.insert(bind_loc);
      glBindAttribLocation(handle_, bind_loc, name);
    } catch(std::runtime_error&) {
      throw ResourceLoadError("invalid atttrib setup");
    }
  }
}

void Program::resolveUniformBuffers_(ResourceManager* manager) {
  GLint max_name_len = 0;
  GLint blocks_count = 0;

  glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
                 &max_name_len);
  glGetProgramiv(handle_, GL_ACTIVE_UNIFORM_BLOCKS, &blocks_count);

  std::vector<char> block_name(max_name_len);
  for(GLint i = 0; i < blocks_count; ++i) {
    GLsizei len = 0;
    glGetActiveUniformBlockName(handle_, i, max_name_len, &len,
                                block_name.data());
    auto layout = manager->getUniformBlock(StringView(block_name.data(), len));
    if(!layout) {
      throw ResourceLoadError(
          "Program uses non-registered uniform buffer layout");
    }
    glUniformBlockBinding(handle_, i, layout.handle_cache_);
  }
}

Program::~Program() {
  if(cache_) {
    cache_->remove(this);
  }
  glDeleteProgram(handle_);
}
}
}
namespace {
GLuint bound_program = 0;
}

namespace render {
ProgramUsage::ProgramUsage(ProgramRef const& p) {
  SLT_ASSERT(!bound_program);
  glUseProgram(p.handle_cache_);
  bound_program = p.handle_cache_;
}

ProgramUsage::~ProgramUsage() {
  bound_program = 0;
}
}
}