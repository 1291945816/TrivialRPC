/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/14 10:14:10
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "Client.h"
#include "base/Logger.h"
#include "common.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

Client::Client(int file_desc) {
	sock_fd_.set(file_desc);
	set_connected(true);
}

void Client::publishEvent(ClientEvent clientEvent, const std::string& msg) {
	handler_callback_(*this, clientEvent, msg);
}

void Client::send(const char* msg, size_t msgSize) const {
	const auto ret = ::send(sock_fd_.get(), msg, msgSize, 0);
	if (ret < 0) {
		throw std::runtime_error(strerror(errno));
	}
	// 处理 发送的消息 < 实际长度
	if (ret < msgSize) {
		char err_msg[100];
		sprintf(err_msg, "Only %lu bytes out of %lu was sent to client", ret,
		        msgSize);
		throw std::runtime_error(err_msg);
	}
}

bool Client::receive_data() {

	std::string msg;
	while (is_connected()) {

		char rec_buf[MAX_PACKET_SIZE] = {'\0'};
		auto numofBytes_rec =
		    ::recv(sock_fd_.get(), &rec_buf, MAX_PACKET_SIZE, 0);
		if (numofBytes_rec < 1) {
			std::string disconnect_msg;
			if (numofBytes_rec < 0) {
				// 读取完毕
				if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
					publishEvent(ClientEvent::INCOMING_MSG, msg);
					return true;
				} else {
					// -1 发生错误
					disconnect_msg = strerror(errno);
				}
			} else { // 为 0
				disconnect_msg = "Client has closed connection!";
			}

			set_connected(false);
			publishEvent(ClientEvent::DISCONNECTED, disconnect_msg);
			return false;
		} else {
			msg.append(rec_buf);
		}
	}
	return true;
}

void Client::close() {
	DEBUG_LOG << "已关闭连接: " << this->get_ip();
	set_connected(false);
	const auto ret = ::close(sock_fd_.get());
	if (ret == -1)
		throw std::runtime_error(strerror(errno));
}

void Client::print() const {
	const std::string connected = is_connected() ? "True" : "False";
	INFO_LOG << "-----------------";
	INFO_LOG << "IP address: " << get_ip();
	INFO_LOG << "Connected?: " << connected;
	INFO_LOG << "Socket FD: " << sock_fd_.get();
}

ssize_t Client::recv(void* buffer, size_t length, int flag) {
	if (is_connected()) {
		return ::recv(sock_fd_.get(), buffer, length, flag);
	}
	return -1;
}

ssize_t Client::recv(iovec* buffers, size_t length, int flags) {
	if (is_connected()) {
		msghdr msg;
		memset(&msg, 0, sizeof(msg));
		msg.msg_iov = (iovec*)buffers;
		msg.msg_iovlen = length;
		return ::recvmsg(sock_fd_.get(), &msg, flags);
	}
	return -1;
}

ssize_t Client::send(const void* buffer, size_t length, int flag) {
	if (is_connected()) {
		return ::send(sock_fd_.get(), buffer, length, flag);
	}
	return -1;
}
ssize_t Client::send(const iovec* buffers, size_t length, int flag) {
	if (is_connected()) {
		msghdr msg;
		memset(&msg, 0, sizeof(msg));
		msg.msg_iov = (iovec*)buffers;
		msg.msg_iovlen = length;
		return ::sendmsg(sock_fd_.get(), &msg, flag);
	}
	return -1;
}
