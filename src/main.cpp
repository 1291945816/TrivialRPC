#include "ServerObserver.h"
#include "TcpServer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>

TcpServer server;
ServerObserver observer;

void onIncomingMsg(const std::string& clientIP, const char* msg, size_t size) {
	// print client message
	std::cout << "Observer1 got client msg: " << msg << "\n";
	server.send_to_client(clientIP, msg, strlen(msg));
}

void onClientDisconnected(const std::string& ip, const std::string& msg) {
	std::cout << "Client: " << ip << " disconnected. Reason: " << msg << "\n";
}

int main() {
	const auto ret = server.start(8080); // 启动端口
	if (ret.is_successful())
		std::cout << "server setup successfully!\n";
	else {
		std::cout << "server setup failed: " << ret.message() << "\n";
		return EXIT_FAILURE;
	}
	observer.incoming_packet_handler_ = onIncomingMsg;
	observer.disconnection_handler_ = onClientDisconnected;
	server.subscribe(observer);
	try {
		std::cout << "waiting for incoming client...\n";
		server.run();
	} catch (const std::runtime_error& error) {
		std::cout << "Accepting client failed: " << error.what() << "\n";
	}

	return 0;
}