file(GLOB SRC_SOURCE ./* )

add_subdirectory(./base)
add_library(${PROJECT_NAME} ${SRC_SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE base)