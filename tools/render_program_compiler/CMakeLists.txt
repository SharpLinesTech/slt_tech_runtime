add_executable(render_program_compiler 
  main.cpp)

target_link_libraries(render_program_compiler slt_tech_core)

SLT_TOOL_CONFIG(render_program_compiler)