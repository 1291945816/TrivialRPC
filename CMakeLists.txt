cmake_minimum_required(VERSION 3.20)
project(tcpsever)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE "Debug")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
# 配置日志宏 默认日志是Debug级别，仅支持INFO级别、DEBUG的调整 
# 取消该注释则是 INFO级别的日志
# add_definitions(-DINFO_FLAG)

set(LINK_ARGS inicpp)

# 设定编译生成的库目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
message("------------ LIBRARY_OUTPUT_PATH -------------")
message(${PROJECT_SOURCE_DIR}/bin/lib)


include_directories(${PROJECT_SOURCE_DIR}/include)
# 引入第三方的头文件
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/inifile-cpp/include) 

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)

# 第三方模块
add_subdirectory(thirdparty/inifile-cpp)


