file(GLOB BASE_SOURCE ./*.cpp)
add_library(rpc ${BASE_SOURCE})
target_link_libraries(rpc PUBLIC inicpp)