/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/03 11:20:36
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef RPCSERVER_H
#define RPCSERVER_H

#include "FileDescriptor.h"
#include "RPCCommon.h"
#include "TcpServer.h"
#include "base/Logger.h"
#include "base/traits.h"
#include "rpc/Protocol.h"
#include "rpc/Serializer.h"
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

class RPCServer : public TcpServer {
public:
	RPCServer() = default;
	~RPCServer();
	bool start(int port);
	ResultType close() override;

	/**
	 * @brief 向RPC服务器注册函数
	 *
	 * @tparam Func
	 * @param name 注册的函数名称
	 * @param func 注册的函数
	 */
	template <typename Func>
	void registerMethod(const std::string& name, Func func) {
		DEBUG_LOG << "rpc server register method: " << name;
		handlers_[name] = [func, this](Serializer serializer,
		                               const std::string& arg) {
			proxy(func, serializer, arg);
		};
	}

protected:
	/**
	 * @brief 代理函数接口
	 *
	 * @tparam F 函数类型
	 * @param fun
	 * @param serializer
	 * @param arg
	 */
	template <typename F>
	void proxy(F fun, Serializer serializer, const std::string& arg) {
		typename function_traits<F>::stl_function_type func(fun);
		using Return = typename function_traits<F>::return_type;
		using Args = typename function_traits<F>::tuple_type;
		Serializer s(arg);
		Args args;
		try {
			s >> args;
		} catch (...) {
			RPCResult<Return> val;
			val.setCode(RPC_NO_MATCH);
			val.setMsg("params not match");
			serializer << val;
			return;
		}

		ReturnType<Return> rt{};
		constexpr auto size =
		    std::tuple_size<typename std::decay<Args>::type>::value;
		// 匿名lambda函数
		auto invoke = [&func, &args]<std::size_t... Index>(
		                  std::index_sequence<Index...>) {
			return func(std::get<Index>(std::forward<Args>(args))...);
		};
		if constexpr (std::is_same_v<Return, void>) {
			invoke(std::make_index_sequence<size>{});
		} else {
			rt = invoke(std::make_index_sequence<size>{});
		}
		RPCResult<Return> val;
		val.setCode(RPC_SUCCESS);
		val.setVal(rt);
		serializer << val;
	}

	void publish_client_msg(const Client& client, const char* msg,
	                        size_t msg_size) override;
	void publish_client_disconnected(const std::string& client_ip,
	                                 const std::string& msg) override;
	/**
	 * @brief 调用服务端注册的函数，返回序列化的结果
	 *
	 * @param name 函数名称
	 * @param arg 函数参数字节流
	 * @return Serializer
	 */
	Serializer call(const std::string& name, const std::string& arg);
	/**
	 * @brief 用于处理方法调用
	 * @param proto
	 * @return Protocol::ptr
	 */
	Protocol::ptr handleMethodCall(Protocol::ptr proto);

private:
	int port_; // 开放服务端口
	std::map<std::string, std::function<void(Serializer, const std::string&)>>
	    handlers_; // 注册的函数
};

#endif // RPCSERVER_H
