/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/09 09:37:59
 * @version: 1.0
 * @description:
 ********************************************************************************/
#include "rpc/RPCServer.h"
#include "base/ByteArray.h"
#include "base/Logger.h"
#include "base/util.h"
#include "inicpp.h"
#include "net/Client.h"
#include "net/ResultType.h"
#include "net/TcpServer.h"
#include "net/common.h"
#include "rpc/Protocol.h"
#include "rpc/RPCCommon.h"
#include "rpc/RPCSession.h"
#include "rpc/Serializer.h"
#include <bits/types/struct_iovec.h>
#include <cstring>
#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <zookeeper/zookeeper.h>

static std::string PROVIDER_NAME = "rpc-provider";

RPCServer::RPCServer(ini::IniFile& file) {
	port_ = file["rpc_server"]["port"].as<int>(); // 获取对应的地址
	int max_client_nums = file["rpc_server"]["max_client_nums"].as<int>();
	TcpServer::start(port_,
	                 max_client_nums); // 绑定对应端口 并开始配置线程池的数目
	// 连接zk
	zkclient_.start();
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
	DEBUG_LOG << "call: [" << func_name << "],args:[" << request.toString()
	          << "]";
	Serializer rt = call(func_name, request.toString());
	Protocol::ptr resp =
	    Protocol::Create(Protocol::MsgType::RPC_METHOD_RESPONSE, rt.toString(),
	                     proto->getSequenceId());
	return resp;
}

void RPCServer::publish_client_msg(Client::ptr client, ByteArray::ptr bt) {
	INFO_LOG << "handle from:" << client->get_ip() << " msg";
	Protocol::ptr proto = std::make_shared<Protocol>();
	RPCSession::ptr session_ = std::make_shared<RPCSession>(client);

	// 读取协议
	try {
		proto->decode(bt);
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
	if (response && client->is_connected()) {
		DEBUG_LOG << "submit task."
		          << " " << response->encode()->toHexString();
		// 需要注意这里的传 指针的方式 要考虑作用域 会不会被释放了
		threadpool->submit([client, response, session_, this]() {
			std::lock_guard<std::mutex> lock_(
			    *(client_write_mtx_[client->get_filedesc()])); // 获取对应的写锁
			auto size = session_->sendProtocol(response);
			if (size <= 0)
				ERROR_LOG << "data send failed.";
		});
	}
}

void RPCServer::publish_client_disconnected(Client::ptr client,
                                            ByteArray::ptr bt) {
	INFO_LOG << "[" << client->get_ip()
	         << "]has disconnected: " << bt->readStringF32();
}

void RPCServer::registerService(std::string service_name) {
	std::vector<std::string> paths = parse_path(service_name);
	int i = 0;
	std::string res_service_name{"/"};
	/*处理永久节点*/
	while (i < paths.size()) {
		res_service_name += paths[i] + "/";
		if (!zkclient_.check_znode_exists(res_service_name.c_str())) {
			INFO_LOG  << "znode is not exist,path: "<<res_service_name;
			zkclient_.create(res_service_name.c_str(), "", 0); // 永久节点
		}
		++i;
	}
	//创建临时节点
	res_service_name += PROVIDER_NAME;
	// 获取主机ip
	auto ip = get_local_ip();
	std::string data = ip+":"+std::to_string(port_);
	zkclient_.create(res_service_name.c_str(),data.c_str(), data.size(),ZOO_EPHEMERAL_SEQUENTIAL);
}