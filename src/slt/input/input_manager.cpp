#include "slt/input/input_manager.h"
#include "slt/log/log.h"

namespace slt {
namespace input {
InputManager::InputManager() {}

void InputManager::commit(float dt) {
  for(auto& i : binary_inputs_) {
    i->commit();
  }

  for(auto& i : float_inputs_) {
    i->commit();
  }
}

void Command::assign(Input* inp) {
  input_ = inp;
}

bool BinaryCommand::getPressed() {
  if(input_) {
    return input_->getPressed();
  }
  return false;
}

bool BinaryCommand::getHeld() {
  if(input_) {
    return input_->getHeld();
  }
  return false;
}

bool BinaryCommand::getReleased() {
  if(input_) {
    return input_->getReleased();
  }
  return false;
}

float FloatCommand::getFloatValue() {
  if(input_) {
    return input_->getFloatValue();
  }
  return 0.0f;
}

BinaryInput* InputManager::addBinaryInput(StringView name) {
  slt::log->info("Registering binary input: {}", name);
  auto new_input = new BinaryInput();
  binary_inputs_.emplace_back(new_input);
  auto emplaced = inputs_.emplace(name.toString(), new_input);
  SLT_ASSERT(emplaced.second);
  return new_input;
}

FloatInput* InputManager::addFloatInput(StringView name) {
  slt::log->info("Registering float input: {}", name);
  auto new_input = new FloatInput();
  float_inputs_.emplace_back(new_input);
  auto emplaced = inputs_.emplace(name.toString(), new_input);
  SLT_ASSERT(emplaced.second);
  return new_input;
}

BinaryCommand* InputManager::addBinaryCommand(StringView name) {
  slt::log->info("Registering binary command: {}", name);
  auto new_cmd = new BinaryCommand();
  binary_commands_.emplace_back(new_cmd);
  auto emplaced = commands_.emplace(name.toString(), new_cmd);
  SLT_ASSERT(emplaced.second);
  return new_cmd;
}

FloatCommand* InputManager::addFloatCommand(StringView name) {
  slt::log->info("Registering float command: {}", name);
  auto new_cmd = new FloatCommand();
  float_commands_.emplace_back(new_cmd);
  auto emplaced = commands_.emplace(name.toString(), new_cmd);
  SLT_ASSERT(emplaced.second);
  return new_cmd;
}

void InputManager::bind(StringView command, StringView input) {
  auto cmd = commands_.find(command);
  auto inp = inputs_.find(input);

  SLT_ASSERT(cmd != commands_.end());
  SLT_ASSERT(inp != inputs_.end());

  cmd->second->assign(inp->second);
}

void BinaryInput::update(bool value) {
  update_val = value;
}

void BinaryInput::commit() {
  is_released_ = is_held_ && !update_val;
  is_pressed_ = update_val && !is_held_;
  is_held_ = update_val;
}

bool BinaryInput::getPressed() {
  return is_pressed_;
}

bool BinaryInput::getHeld() {
  return is_held_;
}

bool BinaryInput::getReleased() {
  return is_released_;
}

float BinaryInput::getFloatValue() {
  return is_held_ ? 1.0f : 0.0f;
}

void FloatInput::commit() {
  previous_val_ = current_val_;
  current_val_ = update_val_;
}

bool FloatInput::getPressed() {
  return previous_val_ < 0.5f && current_val_ >= 0.5f;
}

bool FloatInput::getHeld() {
  return current_val_ >= 0.5f;
}

bool FloatInput::getReleased() {
  return previous_val_ >= 0.5f && current_val_ < 0.5f;
}

float FloatInput::getFloatValue() {
  return current_val_;
}

void FloatInput::update(float val) {
  update_val_ = val;
}
}
}