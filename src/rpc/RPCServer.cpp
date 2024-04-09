#include "rpc/RPCServer.h"
#include "TcpServer.h"
#include "base/ByteArray.h"
#include "base/Logger.h"
#include "rpc/Protocol.h"
#include "rpc/RPCSession.h"
#include "rpc/Serializer.h"
#include <cstring>
#include <exception>
#include <memory>
#include <mutex>
#include <string>

bool RPCServer::start(int port) {
	TcpServer::start(port);
	return true;
}
ResultType RPCServer::close() { return TcpServer::close(); }
RPCServer::~RPCServer() { close(); }

Serializer RPCServer::call(const std::string& name, const std::string& arg) {
	Serializer serializer;
	if (handlers_.count(name) == 0) {
		return serializer;
	}
	auto func = handlers_[name];
	func(serializer, arg);
	serializer.reset();
	return serializer;
}

Protocol::ptr RPCServer::handleMethodCall(Protocol::ptr proto) {
	std::string func_name;
	Serializer request(proto->getContent());
	request >> func_name;
	DEBUG_LOG << "call: [" << func_name << "]";
	Serializer rt = call(func_name, request.toString());
	Protocol::ptr resp =
	    Protocol::Create(Protocol::MsgType::RPC_METHOD_RESPONSE, rt.toString(),
	                     proto->getSequenceId());
	return resp;
}

void RPCServer::publish_client_msg(const Client& client, const char* msg,
                                   size_t msg_size) {
	Protocol::ptr proto = std::make_shared<Protocol>();
	ByteArray::ptr byte_arr = std::make_shared<ByteArray>();
	byte_arr->writeStringWithoutLength(std::string(msg));
	byte_arr->setPosition(0);

	// 读取协议
	try {
		proto->decode(byte_arr);
	} catch (std::exception& err) {
		ERROR_LOG << err.what();
		return;
	}
	if (proto->getMagic() != Protocol::MAGIC) {
		ERROR_LOG << "There is a problem with this serialized data.";
		return;
	}
	Protocol::ptr response;
	Protocol::MsgType type = proto->getMsgType();
	switch (type) {
	case Protocol::MsgType::RPC_METHOD_REQUEST: {
		response = handleMethodCall(proto);
		break;
	}
	}
	// 加入线程池进行处理
	if (response && client.is_connected()) {
		auto str = response->encode()->toString();
		threadpool->submit([&client, &str, this]() {
			std::lock_guard<std::mutex> lock_(
			    *(client_write_mtx_[client.get_filedesc()])); // 获取对应的写锁
			this->send_to_client(client, str.c_str(), str.size());
		});
	}
}

void RPCServer::publish_client_disconnected(const std::string& client_ip,
                                            const std::string& msg) {
	INFO_LOG << client_ip << "has disconnected: " << msg;
}
