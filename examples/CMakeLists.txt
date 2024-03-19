# 构建tcp sever测试
set(TCP_EXAMPLE tcpserver_demo.cpp)
add_executable(server ${TCP_EXAMPLE})
target_link_libraries(server PRIVATE tcpsever)

