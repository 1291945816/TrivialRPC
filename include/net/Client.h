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
#include "base/ByteArray.h"
#include <atomic>
#include <bits/types/struct_iovec.h>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <sys/types.h>
#include <thread>

enum class ClientEvent {
	DISCONNECTED, // 失去连接
	INCOMING_MSG  // 收到消息
};

class Client:public std::enable_shared_from_this<Client> {
	using client_event_handler_t =
	    std::function<void(std::shared_ptr<Client>, ClientEvent, ByteArray::ptr )>;
public:
	using ptr = std::shared_ptr<Client>;
	Client(int);
	bool operator==(const Client& other) const {
		return (this->sock_fd_.get() == other.sock_fd_.get()) &&
		       (this->ip_ == other.ip_);
	};
	void set_ip(const std::string& ip) { ip_ = ip; }
	std::string get_ip() const { return ip_; }

	const FileDescriptor& get_filedesc() const {return sock_fd_;}

	void set_event_handler(const client_event_handler_t& event_handle) {
		handler_callback_ = event_handle;
	}

	void publishEvent(ClientEvent clientEvent, const std::string& msg = "");
	void publishEvent(ClientEvent clientEvent, ByteArray::ptr bt);

	bool is_connected() const { return is_connected_; }
	/**
	 * @brief 设置对象的连接情况
	 *
	 * @param flag
	 */
	void set_connected(bool flag) { is_connected_ = flag; }

	/**
	 * @brief 接收来自客户端的数据
	 * 目前未加入处理数据的过程，只要来数据就进行接收，直到缓冲区没有数据
	 * @return true 接收完毕 重置状态
	 * @return false 出错 关闭链接 无须处理 由后台程序负责
	 */
	bool receive_data();

	ssize_t recv(void* buffer, size_t length, int flag = 0);
	ssize_t recv(iovec* buffers, size_t length, int flags = 0);

	ssize_t send(const void* buffer, size_t length,int flag=0) const;
	ssize_t send(const iovec* buffers, size_t length, int flag = 0) const;

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
