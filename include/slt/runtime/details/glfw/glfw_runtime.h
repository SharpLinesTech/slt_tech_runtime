#ifndef SLT_GLFW_RUNTIME_IMPLEMENTATION_H
#define SLT_GLFW_RUNTIME_IMPLEMENTATION_H

#include "slt/render/details/opengl/gl_includes.h"
#include "slt/runtime/details/glfw/controller.h"

namespace slt {
namespace runtime_ {
class GLFWRuntime {
 public:
  GLFWwindow* _window = nullptr;

 protected:
  void setupControllers_(input::InputManager* inputs);
  std::vector<std::unique_ptr<glfw::Controller>> controllers;
};

using Implementation = GLFWRuntime;
}
}

#endif