/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/03 11:20:36
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef RPCSERVER_H
#define RPCSERVER_H

#include "RPCCommon.h"
#include "base/ByteArray.h"
#include "base/Logger.h"
#include "base/traits.h"
#include "net/Client.h"
#include "net/FileDescriptor.h"
#include "net/TcpServer.h"
#include "rpc/Protocol.h"
#include "rpc/Serializer.h"
#include "inicpp.h"
#include "rpc/ZKClient.h"
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
	// 引入使用配置文件进行处理
	RPCServer(ini::IniFile& ini);
	~RPCServer();
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
	
	/**
	 * @brief 作为服务提供者，向zk注册服务
	 * @param service_name 注册的服务名称 这个永久的服务名称
	 */
	void registerService(std::string service_name);

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

	void publish_client_msg(Client::ptr client, ByteArray::ptr) override;
	void publish_client_disconnected(Client::ptr client,
	                                 ByteArray::ptr) override;
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
	ZKClient zkclient_{}; // 客户端 只要会话存在 则保证 下线自动销毁对应的节点
};

#endif // RPCSERVER_H
