#include "ServerObserver.h"
#include "TcpServer.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>

TcpServer server;
ServerObserver observer;

void onIncomingMsg(const std::string &clientIP, const char * msg, size_t size) {
    // print client message
    std::cout << "Observer1 got client msg: " << msg << "\n";
}

void onClientDisconnected(const std::string &ip, const std::string &msg) {
    std::cout << "Client: " << ip << " disconnected. Reason: " << msg << "\n";
}

void acceptClient() {
    try {
        std::cout << "waiting for incoming client...\n";
        std::string clientIP = server.accept_client(0); // 0 -> 阻塞等待
        std::cout << "accepted new client with IP: " << clientIP << "\n" <<
                  "== updated list of accepted clients ==" << "\n" << std::endl;
        server.printClients();
        std::fflush(stdout);
    } catch (const std::runtime_error &error) {
        std::cout << "Accepting client failed: " << error.what() << "\n";
    }
}

int main()
{
    const auto ret = server.start(8080);
    if(ret.is_successful())
        std::cout << "server setup successfully!\n";
    else{
        std::cout << "server setup failed: " << ret.message() << "\n";
        return EXIT_FAILURE;
    }
    observer.incoming_packet_handler_ = onIncomingMsg;
    observer.disconnection_handler_ = onClientDisconnected;
    // observer.wanted_ip = "";
    server.subscribe(observer);
    while (1) {
        acceptClient(); // 仅仅处理单个客户端
    }
    
    return 0;
}