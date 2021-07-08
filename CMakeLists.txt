cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME gpu_profiler_core)
project(${TARGET_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(LIBRARY_EXPORTS)

file(GLOB GPC_MAIN_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB GPC_HOOK_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/*.h)
file(GLOB GPC_HOOK_DX_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/dx/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/dx/*.h)
file(GLOB GPC_HOOK_CUDA_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/cuda/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/cuda/*.h)
file(GLOB GPC_HOOK_VK_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/vk/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/hook/vk/*.h)
file(GLOB GPC_UTILS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.h)
file(GLOB GPC_REPORT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/report/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/report/*.h)
file(GLOB GPC_CORE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h)

add_subdirectory(direct_test)
add_subdirectory(tools)

include_directories(./include)
include_directories(./src)
include_directories(./src/hook)
include_directories(./src/core)
include_directories(./src/utils)

link_directories(./libs/detours)
link_directories(./libs/minhook)

source_group("hook" FILES ${GPC_HOOK_SRCS})
source_group("hook/dx" FILES ${GPC_HOOK_DX_SRCS})
source_group("hook/cuda" FILES ${GPC_HOOK_CUDA_SRCS})
source_group("hook/vk" FILES ${GPC_HOOK_VK_SRCS})
source_group("core" FILES ${GPC_CORE_SRCS})
source_group("utils" FILES ${GPC_UTILS_SRCS})
source_group("report" FILES ${GPC_REPORT_SRCS})
source_group("main" FILES ${GPC_MAIN_SRCS})

list(APPEND GPC_HOOK_SRCS ${GPC_HOOK_DX_SRCS} ${GPC_HOOK_CUDA_SRCS} ${GPC_HOOK_VK_SRCS})
list(APPEND GPC_MAIN_SRCS ${GPC_HOOK_SRCS} ${GPC_CORE_SRCS} ${GPC_UTILS_SRCS} ${GPC_REPORT_SRCS})

add_library(${TARGET_NAME} SHARED ${GPC_MAIN_SRCS})
target_link_libraries(${TARGET_NAME} dxgi d3d12 detours minhook.x64)
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

