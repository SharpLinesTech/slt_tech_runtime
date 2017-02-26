cmake_minimum_required(VERSION 3.1)

project(slt_runtime)

SET(FBX_ROOT "" CACHE PATH "Location of the Autodesk FBX SDK")

add_subdirectory(slt_tech_core)
SLT_CORE_BUILD_CONFIG()


SET(SLT_RUNTIME_PUBLIC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(
  src
  ${SLT_RUNTIME_PUBLIC_INCLUDE_DIR}
  ${SLT_TECH_CORE_PUBLIC_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include)

add_subdirectory(src/slt/camera)
add_subdirectory(src/slt/input)
add_subdirectory(src/slt/render)
add_subdirectory(src/slt/runtime)


# Configure thirdparties
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "")
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "")
SET(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "")

# Build thirdparties
add_subdirectory(third_party/glfw)
add_library(glad OBJECT third_party/glad/src/glad.c)

SET(THIRD_PARTY_TARGETS
  glad glfw
)

foreach(TPT ${THIRD_PARTY_TARGETS})
  set_target_properties(${TPT} PROPERTIES FOLDER "third_party")
  set_target_properties(${TPT} PROPERTIES DEBUG_POSTFIX _d)
  set_target_properties(${TPT} PROPERTIES VALIDATE_POSTFIX _v)
endforeach()

add_library(slt_runtime_glfw
  $<TARGET_OBJECTS:glad>
  $<TARGET_OBJECTS:slt_input>
  $<TARGET_OBJECTS:slt_render>
  $<TARGET_OBJECTS:slt_render_opengl>
  $<TARGET_OBJECTS:slt_runtime_common>
  $<TARGET_OBJECTS:slt_runtime_glfw_impl>
)

set_target_properties(slt_runtime_glfw PROPERTIES FOLDER "libs")

add_subdirectory(tools)

install(TARGETS slt_runtime_glfw
  ARCHIVE DESTINATION lib/$<CONFIG>
  RUNTIME DESTINATION bin/$<CONFIG>
  PUBLIC_HEADER DESTINATION include
)

# Install public headers
install(DIRECTORY include/slt/
  DESTINATION include/slt
  FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY third_party/glad/include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h"
)
