cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME hook_test)
project(${TARGET_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

#file(GLOB_RECURSE HOOK_TEST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h* ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hook/*)
file(GLOB_RECURSE HOOK_TEST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)

add_subdirectory(../../external/minhook ${CMAKE_CURRENT_BINARY_DIR}/lib)

#include_directories(../../src)
#include_directories(../../src/utils)
#include_directories(../../src/hook)
#include_directories(../../src/core)
include_directories(../../external/minhook/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

add_executable(${TARGET_NAME} ${HOOK_TEST_SRCS})
target_link_libraries(${TARGET_NAME} minhook.x64)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "direct_test")
add_dependencies(${TARGET_NAME} minhook)
