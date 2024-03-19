cmake_minimum_required(VERSION 3.20)
project(tcpsever)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置日志宏 默认日志是Debug级别，仅支持INFO级别、Debug的调整 
# 取消该注释则是 INFO级别的日志
# add_definitions(-DINFO_FLAG)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(examples)
