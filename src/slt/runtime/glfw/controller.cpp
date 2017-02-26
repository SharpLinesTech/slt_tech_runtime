#include "slt/runtime/details/glfw/controller.h"

#include "slt/render/details/opengl/gl_includes.h"

namespace slt {
namespace glfw {
Controller::Controller(int joy_id, input::InputManager* input_manager)
    : joy_id_(joy_id) {
  int axes_count = 0;
  glfwGetJoystickAxes(joy_id, &axes_count);

  int buttons_count = 0;
  glfwGetJoystickButtons(joy_id, &buttons_count);

  axes_inputs.resize(axes_count);
  btns_inputs.resize(buttons_count);
  for(int i = 0; i < axes_count; ++i) {
    axes_inputs[i] = input_manager->addFloatInput(
        fmt::format("Controller {}. axis {}", joy_id, i));
  }

  for(int i = 0; i < buttons_count; ++i) {
    btns_inputs[i] = input_manager->addBinaryInput(
        fmt::format("Controller {}. btn {}", joy_id, i));
  }
}

Controller::~Controller() {}

void Controller::update() {
  int axes_count = 0;
  auto axes = glfwGetJoystickAxes(joy_id_, &axes_count);

  int buttons_count = 0;
  auto btns = glfwGetJoystickButtons(joy_id_, &buttons_count);

  for(int i = 0; i < axes_count; ++i) {
    axes_inputs[i]->update(axes[i]);
  }

  for(int i = 0; i < buttons_count; ++i) {
    btns_inputs[i]->update(btns[i] == GLFW_PRESS);
  }
}
}
}
