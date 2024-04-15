/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/09 09:39:07
* @version: 1.0
* @description: 
********************************************************************************/ 
#include "net/Client.h"
#include "net/ServerObserver.h"
#include "net/TcpServer.h"
#include "base/ByteArray.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <base/Logger.h>
TcpServer server;
ServerObserver observer;

void onIncomingMsg(const std::string& clientIP, ByteArray::ptr bt) {
	// print client message
	std::cout << "Observer1 got client msg: " << bt->toString() << "\n";
	server.send_to_client(clientIP, bt->toString().c_str(), bt->getReadSize());
}

void onClientDisconnected(const std::string& clientIP, ByteArray::ptr bt) {
	std::cout << "Client: " << clientIP << " disconnected. Reason: " << bt->readStringF32() << "\n";
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