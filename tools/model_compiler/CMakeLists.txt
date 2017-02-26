include_directories(${FBX_ROOT}/include)

add_executable(model_compiler 
  fbx_memory_stream.h
  main.cpp
  mesh_builder.h mesh_builder.cpp
  skeleton_builder.h skeleton_builder.cpp
  )

target_link_libraries(model_compiler 
  slt_tech_core 
  ${FBX_ROOT}/lib/vs2015/x64/release/libfbxsdk.lib)

SLT_TOOL_CONFIG(model_compiler)

if (MSVC)
  install(FILES ${FBX_ROOT}/lib/vs2015/x64/release/libfbxsdk.dll DESTINATION bin/$<CONFIG>)
endif()