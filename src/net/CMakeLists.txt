file(GLOB SRC_SOURCE ./*.cpp )
add_library(net ${SRC_SOURCE})
target_link_libraries(net  PRIVATE base)
target_link_libraries(rpc PUBLIC inicpp)
