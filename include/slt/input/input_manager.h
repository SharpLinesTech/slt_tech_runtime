#ifndef SL_T_INPUT_INPUT_MANAGER_H
#define SL_T_INPUT_INPUT_MANAGER_H

#include <map>
#include "slt/refl/refl.h"
#include "slt/string/string_view.h"

namespace slt {

namespace input {

class Input {
 public:
  virtual bool getPressed() = 0;
  virtual bool getHeld() = 0;
  virtual bool getReleased() = 0;
  virtual float getFloatValue() = 0;
};

class Command {
 public:
  void assign(Input*);

 protected:
  Input* input_;
};

class BinaryCommand : public Command {
 public:
  bool getPressed();
  bool getHeld();
  bool getReleased();
};

class FloatCommand : public Command {
 public:
  float getFloatValue();
};

class BinaryInput : public Input {
 public:
  void update(bool value);
  void commit();

  bool getPressed() override;
  bool getHeld() override;
  bool getReleased() override;
  float getFloatValue() override;

 private:
  bool update_val = false;

  bool is_pressed_ = false;
  bool is_held_ = false;
  bool is_released_ = false;
};

class FloatInput : public Input {
 public:
  void update(float val);
  void commit();
  bool getPressed() override;
  bool getHeld() override;
  bool getReleased() override;
  float getFloatValue() override;

 private:
  float update_val_ = 0.0f;

  float previous_val_ = 0.0f;
  float current_val_ = 0.0f;
};

class InputManager {
 public:
  InputManager();

  void commit(float dt);

  BinaryInput* addBinaryInput(StringView);
  FloatInput* addFloatInput(StringView);

  BinaryCommand* addBinaryCommand(StringView);
  FloatCommand* addFloatCommand(StringView);

  void bind(StringView command, StringView input);

 private:
  std::map<std::string, Command*, std::less<>> commands_;
  std::map<std::string, Input*, std::less<>> inputs_;

  std::vector<std::unique_ptr<BinaryInput>> binary_inputs_;
  std::vector<std::unique_ptr<FloatInput>> float_inputs_;
  std::vector<std::unique_ptr<BinaryCommand>> binary_commands_;
  std::vector<std::unique_ptr<FloatCommand>> float_commands_;

  std::map<std::string, std::string, std::less<>> pending_bindings_;
};
}
}

#endif