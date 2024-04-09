/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/08 09:57:20
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef RPCCOMMON_H
#define RPCCOMMON_H

#include "ResultType.h"
#include "Serializer.h"
#include <cstdint>
#include <string>
template <typename T>
struct return_type {
	using type = T;
};

/**
 * @brief 单独考虑调用结果是void的 将类型转换为int8
 * @tparam
 */
template <>
struct return_type<void> {
	using type = int8_t;
};

template <typename T>
using ReturnType = typename return_type<T>::type;

enum RPCState {
	RPC_SUCCESS = 0, // 成功
	RPC_FAIL,        // 失败
	RPC_NO_MATCH,    // 函数不匹配
	RPC_NO_METHOD,   // 没有找到调用函数
	RPC_CLOSED,      // RPC连接被关闭
	RPC_TIMEOUT,     // RPC调用超时
};
template <typename T = void>
class RPCResult {
public:
	using RawType = T;
	using Type = ReturnType<T>;
	using MsgType = std::string;
	using CodeType = uint16_t;

	static RPCResult<T> Success() {
		RPCResult<T> res;
		res.setCode(RPC_SUCCESS);
		res.setMsg("success");
		return res;
	}

	static RPCResult<T> Fail() {
		RPCResult<T> res;
		res.setCode(RPC_FAIL);
		res.setMsg("fail");
		return res;
	}
	RPCResult() {}
	bool valid() { return code_ == 0; }
	Type& getVal() { return val_; }
	void setVal(const Type& val) { val_ = val; }
	void setCode(CodeType code) { code_ = code; }
	int getCode() { return code_; }
	void setMsg(MsgType msg) { msg_ = msg; }
	const MsgType& getMsg() { return msg_; }

	Type* operator->() noexcept { return &val_; }
	const Type* operator->() const noexcept { return &val_; }

	/**
	 * @brief 反序列化回 Result
	 *
	 * @param in 序列化的结果
	 * @param d 反序列化回 RPCResult
	 * @return Serializer&
	 */
	friend Serializer& operator>>(Serializer& in, RPCResult<T>& d) {
		in >> d.code_ >> d.msg_;
		if (d.code_ == 0) {
			in >> d.val_;
		}
		return in;
	}

	/**
	 * @brief 将Result序列化
	 *
	 * @param out 序列化输出
	 * @param d 将RPCResult序列化
	 * @return Serializer&
	 */
	friend Serializer& operator<<(Serializer& out, RPCResult<T> d) {
		out << d.code_ << d.msg_ << d.val_;
		return out;
	}

private:
	CodeType code_ = 0;
	MsgType msg_;
	Type val_;
};

#endif // RPCCOMMON_H
