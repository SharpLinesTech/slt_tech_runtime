#include "slt/runtime/details/glfw/glfw_runtime.h"
#include "slt/log/log.h"
#include "slt/render/program.h"
#include "slt/runtime/runtime.h"

namespace {

slt::Setting<bool> enable_opengl_debugging(true, "opengl_debug",
                                           "enable opengl debug logging");
void glfw_error_callback(int error, const char* description) {
  slt::log->error("GLFW error: {}, {}", error, description);
}

void APIENTRY opengl_error_callback(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
                                    const GLchar* message,
                                    const void* userParam) {
  switch(severity) {
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_HIGH:
      slt::log->error("OpenGL error: {}", message);
      break;
    default:
      // slt::log->info("OpenGL message: {}", message);
      break;
  }
}

void joystick_callback(int joy, int event) {
  switch(event) {
    case GLFW_CONNECTED:
      slt::log->info("(GLFW) joystick connected: {}, {}", joy,
                     glfwGetJoystickName(joy));
      break;
    case GLFW_DISCONNECTED:
      slt::log->info("(GLFW) joystick disconnected: {}", joy);
      break;
  }
}
}

namespace slt {

Runtime::Runtime(render::MainRenderTargetConfig const& main_rdt_config,
                 StringView window_name) {
  // There should only ever be a single runtime instance in existence.
  SLT_ASSERT(instance == nullptr);
  instance = this;

  slt::log->info("Initializing platform (GLFW)");
  glfwSetErrorCallback(glfw_error_callback);
  if(!glfwInit()) {
    throw RuntimeInitError("(glfw) init failure.");
  }

  slt::log->info("Creating runtime window (GLFW)");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);

  auto name_as_string = window_name.toString();
  _window = glfwCreateWindow(main_rdt_config.width, main_rdt_config.height,
                             name_as_string.c_str(), nullptr, nullptr);

  if(!_window) {
    glfwTerminate();
    throw RuntimeInitError("(glfw) Failed to create window.");
  }

  glfwMakeContextCurrent(_window);
  glfwSwapInterval(1);

  auto glad_status = gladLoadGL();
  if(!glad_status) {
    glfwDestroyWindow(_window);
    glfwTerminate();
    throw RuntimeInitError(
        "(glad) Failed to initialize opengl function pointers.");
  }

  if(enable_opengl_debugging.get()) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_error_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          true);
  }

  resources.loadDefaultResources();

  setupControllers_(&this->inputs);
}

Runtime::~Runtime() {
  slt::log->info("Tearing down platform (GLFW)");

  glfwDestroyWindow(_window);
  glfwTerminate();

  instance = nullptr;
}

bool Runtime::keepRunning() const {
  return glfwWindowShouldClose(_window) == 0;
}

void Runtime::update(float dt) {
  glfwPollEvents();

  for(auto& ctrl : controllers) {
    if(ctrl) {
      ctrl->update();
    }
  }

  // ehhhh...... not great...
  inputs.commit(dt);
}

void Runtime::finishFrame() {
  glfwSwapBuffers(_window);
}

namespace runtime_ {
void GLFWRuntime::setupControllers_(input::InputManager* inputs) {
  glfwSetJoystickCallback(joystick_callback);

  controllers.resize(GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1);
  for(int joy = GLFW_JOYSTICK_1; joy <= GLFW_JOYSTICK_LAST; ++joy) {
    int present = glfwJoystickPresent(joy);
    if(present) {
      slt::log->info("found joystick: \"{}\", connected at {}",
                     glfwGetJoystickName(joy), joy);
      // Register the controller's inputs:
      controllers[joy] = std::make_unique<glfw::Controller>(joy, inputs);
    }
  }
}
}
}