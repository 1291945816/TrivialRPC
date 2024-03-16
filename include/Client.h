/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/14 09:57:45
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef CLIENT_H
#define CLIENT_H

#include "FileDescriptor.h"
#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <thread>

enum class ClientEvent {
	DISCONNECTED, // 失去连接
	INCOMING_MSG  // 收到消息
};

class Client {
	using client_event_handler_t =
	    std::function<void(const Client&, ClientEvent, const std::string&)>;

private:
	/**
	 * @brief 设置对象的连接情况
	 *
	 * @param flag
	 */
	void set_connected(bool flag) { is_connected_ = flag; }

public:
	Client(int);
	bool operator==(const Client& other) const {
		return (this->sock_fd_.get() == other.sock_fd_.get()) &&
		       (this->ip_ == other.ip_);
	};
	void set_ip(const std::string& ip) { ip_ = ip; }
	std::string get_ip() const { return ip_; }

	void set_event_handler(const client_event_handler_t& event_handle) {
		handler_callback_ = event_handle;
	}

	void publishEvent(ClientEvent clientEvent, const std::string& msg = "");

	bool is_connected() const { return is_connected_; }

	/**
	 * @brief 接收来自客户端的数据
	 * 目前未加入处理数据的过程，只要来数据就进行接收，直到缓冲区没有数据
	 * @return true 接收完毕 重置状态
	 * @return false 出错 关闭链接 无须处理 由后台程序负责
	 */
	bool receive_data();

	// 向客户端发送消息
	void send(const char* msg, size_t msgSize) const;

	/**
	 * @brief 关闭当前连接
	 *
	 */
	void close();
	void print() const;

private:
	FileDescriptor sock_fd_{};        // sock 句柄
	std::string ip_{""};              // 客户端ip地址
	std::atomic_bool is_connected_{}; // 判断是否连接

	// 针对当前客户端的回调 即如何处理获取的消息
	client_event_handler_t handler_callback_;
}; // Client

#endif // CLIENT_H
