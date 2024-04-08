file(GLOB SRC_SOURCE ./*.cpp )

foreach(var ${SRC_SOURCE})
    string(REGEX REPLACE ".*/" "" var ${var})
    string(REGEX REPLACE ".cpp" "" tgt ${var})
    add_executable(${tgt} ${var})
    message("file is: ${tgt}" )
    target_link_libraries(${tgt} PRIVATE tcpsever)
endforeach()