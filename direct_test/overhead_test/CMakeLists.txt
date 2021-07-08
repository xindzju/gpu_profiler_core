cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME overhead_test)
project(${TARGET_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB HOOK_TEST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h*)

add_executable(${TARGET_NAME} ${HOOK_TEST_SRCS})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "direct_test")