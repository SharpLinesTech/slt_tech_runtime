SET(SLT_INPUT_PRIVATE
  input_manager.cpp
)

SET(SLT_INPUT_PUBLIC
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}/slt/input/input_manager.h
)


add_library(slt_input OBJECT
  ${SLT_INPUT_PRIVATE} ${SLT_INPUT_PUBLIC}
)


SLT_MODULE_CONFIG(slt_input)

slt_unit_test(input_manager_test inputs)
target_link_libraries(input_manager_test slt_runtime)