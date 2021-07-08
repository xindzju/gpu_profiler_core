cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME vtable_parser)
project(${TARGET_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB VTABLE_PARSER_TEST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h* ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils/gpc_vtable_parser.* ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils/gpc_utils.* ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core/gpc_struct.h)

include_directories(../../src)
include_directories(../../src/utils)
include_directories(../../src/hook)
include_directories(../../src/core)

add_executable(${TARGET_NAME} ${VTABLE_PARSER_TEST_SRCS})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "tools")