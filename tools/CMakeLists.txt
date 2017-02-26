macro(SLT_TOOL_CONFIG tgt)
  set_target_properties(${tgt} PROPERTIES FOLDER "tools")
  install(TARGETS ${tgt} RUNTIME DESTINATION bin/$<CONFIG>)
endmacro()

add_subdirectory(render_program_compiler)
add_subdirectory(model_compiler)

install(
FILES 
  build/build_assets.py build/build_runner.py 
DESTINATION 
  bin/$<CONFIG>)
  