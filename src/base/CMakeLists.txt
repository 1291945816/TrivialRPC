file(GLOB BASE_SOURCE ./*.cpp)
add_library(base ${BASE_SOURCE})
target_link_libraries(base  PRIVATE Threads::Threads)