# 构建tcp sever测试
set(TCPSERVER_EXAMPLE tcpserver_demo.cpp)
add_executable(server ${TCPSERVER_EXAMPLE})
target_link_libraries(server PRIVATE tcpsever)

set(TCPCLIENT_EXAMPLE tcpclient_demo.cpp)
add_executable(client ${TCPCLIENT_EXAMPLE})
target_link_libraries(client PRIVATE tcpsever)