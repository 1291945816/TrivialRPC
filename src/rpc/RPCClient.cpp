/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/09 10:04:51
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "rpc/RPCClient.h"
#include "base/Logger.h"
#include "net/common.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <random>
#include <string>

RPCClient::RPCClient(ini::IniFile ini_file) {
	service_name_ = ini_file["rpc_client"]["provider_service_name"].as<std::string>();
	zkclient_.start(); // 连接注册中心
	if(zkclient_.check_znode_exists(service_name_.c_str())){
		auto ret = zkclient_.get_children(service_name_.c_str());
		INFO_LOG << ret.size();
		service_info.swap(ret);
	}
	update_ip_info();
	
}

int RPCClient::initialize_socket() {
	sock_fd_.set(socket(AF_INET, SOCK_STREAM, 0));
	const bool socket_failed = (sock_fd_.get() == -1);
	if (socket_failed) {
		throw std::runtime_error(strerror(errno));
	}
	// 非阻塞模式
	int old_socket_flag = fcntl(sock_fd_.get(), F_GETFL, 0);
	int new_socket_flag = old_socket_flag | O_NONBLOCK;
	auto ret = fcntl(sock_fd_.get(), F_SETFL, new_socket_flag);
	if (ret == -1)
		throw std::runtime_error(strerror(errno));
	return old_socket_flag;
}

void RPCClient::set_address(const std::string& address, int port) {
	auto ret = inet_aton(address.c_str(), &server_.sin_addr);
	// 未解析成功 尝试从字符串中进行解析
	if (!ret) {
		struct hostent* host;
		struct in_addr** addr_list;
		if ((host = gethostbyname(address.c_str())) == nullptr) {
			throw std::runtime_error("Failed to resolve hostname");
		}
		addr_list = (struct in_addr**)host->h_addr_list;
		server_.sin_addr = *addr_list[0];
	}
	server_.sin_family = AF_INET; // ipv4 协议
	server_.sin_port = htons(port);
}

ResultType RPCClient::connect_server() {
	int fdopt = 0;
	try {
		fdopt = initialize_socket();
		set_address(ip_, port_);
	} catch (const std::runtime_error& error) {
		
		return ResultType::FAILURE(error.what());
	}
	client_ = std::make_shared<Client>(sock_fd_.get());
	session_ = std::make_shared<RPCSession>(client_);
	while (true) {
		// 连接服务器
		auto connect_result = connect(
		    sock_fd_.get(), (struct sockaddr*)&server_, sizeof(server_));
		if (connect_result == 0) {
			INFO_LOG << "connect to server[" << ip_ << ":" << port_
			         << "] successfully.";
			is_connected_ = true;
			is_closed_ = false;
			client_->set_connected(true);
			fcntl(sock_fd_.get(), F_SETFL, fdopt);
			return ResultType::SUCCESS();
		} else if (connect_result == -1) {
			if (errno == EINTR) {
				INFO_LOG << "connect to server[" << ip_ << ":" << port_
				         << "] interruptted by signal, try again.";
				continue;
			} else if (errno == EINPROGRESS) {
				// 连接正在重试中
				INFO_LOG << "connect to server[" << ip_ << ":" << port_
				         << "]  try again....";
				break;
			} else {
				return ResultType::FAILURE(strerror(errno));
			}
		}
	}
	// 继续使用select 进行检测 检测可写事件
	auto ret = fd_wait::wait_for_write(sock_fd_.get());
	if (ret != fd_wait::Result::SUCCESS) {

		ERROR_LOG << "connect to server[" << ip_ << ":" << port_ << "] error.";
		return ResultType::FAILURE(strerror(errno));
	}
	int err;
	socklen_t len = static_cast<socklen_t>(sizeof err);
	if (::getsockopt(sock_fd_.get(), SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
		return ResultType::FAILURE(strerror(errno));
	}
	if (err == 0) {
		INFO_LOG << "[select]connect to server[" << ip_ << ":" << port_
		         << "] successfully.";
		is_connected_ = true;
		is_closed_ = false;
		client_->set_connected(true);
		fcntl(sock_fd_.get(), F_SETFL, fdopt);
		return ResultType::SUCCESS();
	} else {
		ERROR_LOG << "connect to server[" << ip_ << ":" << port_ << "] error.";
		return ResultType::FAILURE(strerror(errno));
	}
}

RPCClient::~RPCClient() {
	if (is_closed_) {
		return;
	}
	auto ret = ::close(sock_fd_.get());
	is_closed_ = true;
}

void RPCClient::update_ip_info(){
	static std::random_device rd;
	if(service_info.empty()) return;
	std::uniform_int_distribution<> dist(0,service_info.size()-1);
	// 获取一个随机数
	auto index = dist(rd);
	// 获取到接口的信息
	std::string info = service_info[index];
	auto data = zkclient_.get_data((service_name_+"/"+info).c_str());
	auto pos = data.find(":");
	if(pos == std::string::npos){
		WARNING_LOG << "incoreect service informations";
		return;
	}
	ip_ = data.substr(0,pos);
	port_ = std::stoi(data.substr(pos+1));
	INFO_LOG << "update ip: " << ip_ << ", port: " << port_;
}