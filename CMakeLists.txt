#
# Specify minimum CMake version and project name
#
cmake_minimum_required (VERSION 3.5)
project (Super_Waddle)

#
# CMake setup
#
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_VERBOSE_MAKEFILE 0) # 1 should be used for debugging
set (CMAKE_SUPPRESS_REGENERATION TRUE) # Suppresses ZERO_CHECK
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}, -std=c++20")
set (CMAKE_CXX_STANDARD 20)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++20")
  if(NOT WIN32)
    set(GLAD_LIBRARIES dl)
  endif()
endif()

#
# GLFW options
#
option (GLFW_INSTALL OFF)
option (GLFW_BUILD_DOCS OFF)
option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
add_subdirectory (project_files/vendor/glfw)

#
# Set include paths
#
include_directories (project_files/src/
                     project_files/vendor/glad/include/
                     project_files/vendor/glfw/include/
                     project_files/vendor/glm/
                     project_files/vendor/stb/
                     project_files/vendor/fmt/include)

#
# Add files
#
file (GLOB         VENDORS_SOURCES project_files/vendor/glad/src/glad.c)
file (GLOB_RECURSE PROJECT_HEADERS project_files/src/*.hpp
                                   project_files/src/*.h)
file (GLOB_RECURSE PROJECT_SOURCES project_files/src/*.cpp
                                   project_files/src/*.cxx
                                   project_files/src/*.cc
                                   project_files/src/*.c)
file (GLOB_RECURSE PROJECT_SHADERS project_files/shaders/*.comp
                                   project_files/shaders/*.frag
                                   project_files/shaders/*.geom
                                   project_files/shaders/*.vert)
file (GLOB         PROJECT_CONFIGS CMakeLists.txt
                                   README.rst
                                  .gitignore
                                  .gitmodules)

#
# Organizing files
#
source_group ("headers" FILES ${PROJECT_HEADERS})
source_group ("shaders" FILES ${PROJECT_SHADERS})
source_group ("sources" FILES ${PROJECT_SOURCES})
source_group ("vendors" FILES ${VENDORS_SOURCES})

#
# Set executable and target link libraries
#
add_definitions (-DGLFW_INCLUDE_NONE
                 -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable (${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                                ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                                ${VENDORS_SOURCES})
target_link_libraries (${PROJECT_NAME}
                       glfw
                       ${GLFW_LIBRARIES}
                       ${GLAD_LIBRARIES})
set_target_properties (${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
