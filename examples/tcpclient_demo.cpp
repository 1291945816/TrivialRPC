#include "ClientObserver.h"
#include "ResultType.h"
#include "TcpClient.h"
#include <base/Logger.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <unistd.h>

TcpClient client;
ClientObserver observer;

void accpt_msg(const char* msg, size_t size) {
	DEBUG_LOG << "client get msg: " << msg;
}
void disconnect(const ResultType& ret) {
	DEBUG_LOG << "disconnect:" << ret.message();
}

int main() {
	observer.incoming_packet_handler = accpt_msg;
	observer.disconnection_handler = disconnect;
	client.subscribe(observer);
	bool connected = false;
	while (!connected) {
		auto ret = client.connect_to("127.0.0.1", 8080);
		if (ret.is_successful()) {
			DEBUG_LOG << "connect successfully!";
			client.send_msg("Hello!!", 8);
			connected = true;
            sleep(3);
		} else {
			DEBUG_LOG << "Client failed to connect: " << ret.message() << ","
			          << "Make sure the server is open and listening";
			sleep(3);
			DEBUG_LOG << "Retrying to connect...";
		}
	}
	return 0;
}