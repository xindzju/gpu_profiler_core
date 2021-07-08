cmake_minimum_required(VERSION 3.10)
project(${TARGET_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS On)

add_subdirectory(vtable_parser)

