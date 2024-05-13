/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/09 09:55:37
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef RPCCLIENT_H
#define RPCCLIENT_H

#include "base/Logger.h"
#include "net/Client.h"
#include "net/FileDescriptor.h"
#include "net/ResultType.h"
#include "rpc/Protocol.h"
#include "rpc/RPCCommon.h"
#include "rpc/RPCSession.h"
#include "rpc/Serializer.h"
#include "inicpp.h"
#include "rpc/ZKClient.h"
#include <atomic>
#include <cmath>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

class RPCClient {
public:
	RPCClient(ini::IniFile ini_file);
	~RPCClient();
	ResultType connect_server();

	/**
	 * @brief 有参调用【同步】
	 *
	 * @tparam R
	 * @tparam Params
	 * @param name
	 * @param ps
	 * @return RPCResult<R>
	 */
	template <typename R, typename... Params>
	RPCResult<R> call(const std::string& name, Params... ps) {
		using args_type = std::tuple<typename std::decay_t<Params>...>;
		args_type args = std::make_tuple(ps...);
		Serializer s;
		s << name << args;
		s.reset();
		return call<R>(s);
	}
	/**
	 * @brief 无参调用【同步】
	 *
	 * @tparam R
	 * @param name
	 * @return RPCResult<R>
	 */
	template <typename R>
	RPCResult<R> call(const std::string& name) {
		Serializer s;
		s << name;
		s.reset();
		return call<R>(s);
	}

private:
	int initialize_socket();
	void set_address(const std::string& address, int port);

	template <typename R>
	RPCResult<R> call(Serializer s) {
		RPCResult<R> val;
		if (is_closed_) {
			val.setCode(RPC_CLOSED);
			val.setMsg("socket closed");
			return val;
		}
		auto data = Protocol::Create(Protocol::MsgType::RPC_METHOD_REQUEST,
		                             s.toString());

		auto ret = session_->sendProtocol(data);

		if (ret < 0) {
			val.setCode(RPC_FAIL);
			val.setMsg("have not sent data");
			return val;
		}

		// 接收数据
		auto resp = session_->recvProtocol();
		if (!resp) {
			val.setCode(RPC_FAIL);
			val.setMsg("The parsed data is empty");
			return val;
		}
		INFO_LOG << resp->encode()->toHexString();
		if (resp->getContent().empty()) {
			val.setCode(RPC_NO_METHOD);
			val.setMsg("Method not find");
			return val;
		}

		Serializer serializer(resp->getContent()); // 内容是经过序列化的
		try {
			serializer >> val;
		} catch (...) {
			val.setCode(RPC_NO_MATCH);
			val.setMsg("return value not match");
		}
		return val;
	}
	void update_ip_info();
	

private:
	std::string ip_; // ip地址
	int port_;       // 端口地址	
	FileDescriptor sock_fd_;
	std::shared_ptr<Client> client_;
	std::shared_ptr<RPCSession> session_;
	std::atomic_bool is_connected_{false};
	std::atomic_bool is_closed_{true};
	struct sockaddr_in server_; // 服务器

	/**引入注册中心，订阅对应的服务**/
	ZKClient zkclient_{}; // 注册中心
	std::string service_name_{};
	std::vector<std::string> service_info; // 每一项都是 ip:port


};

#endif // RPCCLIENT_H