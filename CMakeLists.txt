cmake_minimum_required(VERSION 3.20)
project(tcpsever)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC_SOURCE src/* )
add_executable(${PROJECT_NAME} ${SRC_SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)