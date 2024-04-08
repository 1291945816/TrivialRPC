file(GLOB SRC_SOURCE ./*.cpp )

add_subdirectory(./base)
add_subdirectory(./rpc)
add_library(${PROJECT_NAME} ${SRC_SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE base)
target_link_libraries(${PROJECT_NAME} PRIVATE rpc)
