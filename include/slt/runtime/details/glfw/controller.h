#ifndef SLT_RUNTIME_GLFW_CONTROLLER_H
#define SLT_RUNTIME_GLFW_CONTROLLER_H

#include "slt/input/input_manager.h"

namespace slt {
namespace glfw {
class Controller {
 public:
  Controller(int joy_id, input::InputManager* input_manager);
  ~Controller();

  void update();

 private:
  int joy_id_;
  std::vector<input::FloatInput*> axes_inputs;
  std::vector<input::BinaryInput*> btns_inputs;
};
}
}

#endif
