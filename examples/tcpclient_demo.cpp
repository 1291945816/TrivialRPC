/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/09 09:38:56
 * @version: 1.0
 * @description:
 ********************************************************************************/
#include "inicpp.h"
#include "net/ClientObserver.h"
#include "net/ResultType.h"
#include "net/TcpClient.h"
#include <base/Logger.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <unistd.h>

// #define DEBUG_LOG std::cout

void accpt_msg(const char* msg, size_t size) {
	DEBUG_LOG << "client get msg: " << msg;
}
void disconnect(const ResultType& ret) {
	DEBUG_LOG << "disconnect:" << ret.message();
}

int main() {
	ini::IniFile my_ini;
	my_ini.load("./test.ini");
	std::cout << my_ini["tcp_client"]["server_ip"].as<std::string>();
	TcpClient client(my_ini);
	ClientObserver observer;
	observer.incoming_packet_handler = accpt_msg;
	observer.disconnection_handler = disconnect;
	client.subscribe(observer);
	auto ret = client.connect_server();
	if (ret.is_successful()) {
		DEBUG_LOG << "connect successfully!";
		client.send_msg("Hello!!", 8);
	} else {
		DEBUG_LOG << "Client failed to connect: " << ret.message() << ","
		          << "Make sure the server is open and listening";
		DEBUG_LOG << "Retrying to connect...";
	}
	return 0;
}