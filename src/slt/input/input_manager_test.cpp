#include "gtest/gtest.h"

#include "slt/core/core.h"
#include "slt/input/input_manager.h"
#include <chrono>



TEST(InputManager, BasicBoolCommandUsage) {
  slt::Core core;
  slt::input::InputManager man;

  auto cmd = man.addBinaryCommand("test_cmd");
  EXPECT_FALSE(cmd->getPressed());

  auto input = man.addBinaryInput("test_input");
  
  man.bind("test_cmd", "test_input");


  // updating the input should not affect the bound command (yet)
  input->update(true);
  EXPECT_FALSE(cmd->getPressed());

  man.commit(0.01f);
  EXPECT_TRUE(cmd->getPressed());

}



TEST(InputManager, DelayedControllerActivation) {
  slt::Core core;
  slt::input::InputManager man;

  auto cmd = man.addBinaryCommand("test_cmd");
  man.bind("test_cmd", "test_input");

  auto input = man.addBinaryInput("test_input");

  EXPECT_FALSE(cmd->getPressed());
  input->update(true);
  EXPECT_FALSE(cmd->getPressed());

  man.commit(0.01f);
  EXPECT_TRUE(cmd->getPressed());

}

