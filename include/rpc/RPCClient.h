/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/09 09:55:37
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef RPCCLIENT_H
#define RPCCLIENT_H


#include "net/Client.h"
#include "base/Logger.h"
#include "net/FileDescriptor.h"
#include "rpc/RPCSession.h"
#include "net/ResultType.h"
#include "rpc/Protocol.h"
#include "rpc/RPCCommon.h"
#include "rpc/Serializer.h"
#include <atomic>
#include <cmath>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <tuple>
#include <type_traits>


class RPCClient {
public:
    ~RPCClient();
	ResultType connect_to(const std::string& address, int port,
	                      int timeout = 3);

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
	int  initialize_socket();
	void set_address(const std::string& address, int port);
	ResultType send_request(const char* msg, size_t size);

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

		INFO_LOG <<"data size: "<< data->encode()->toString();
		// 发送数据
		// auto ret = send_request(data.c_str(), data.size());

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

private:
	FileDescriptor sock_fd_;
	std::shared_ptr<Client> client_;
	std::shared_ptr<RPCSession> session_;
	std::atomic_bool is_connected_{false};
	std::atomic_bool is_closed_{true};
	struct sockaddr_in server_; // 服务器
};

#endif // RPCCLIENT_H