/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/20 10:21:09
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "ClientObserver.h"
#include "FileDescriptor.h"
#include "ResultType.h"
#include "base/Buffer.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <vector>
class TcpClient {

public:
	TcpClient()=default;
	~TcpClient();
	/**
	 * @brief 连接到指定ip地址的服务器
	 *
	 * @param address
	 * @param port
	 * @param timeouy 超时设置 默认是3s
	 * @return ResultType
	 */
	ResultType connect_to(const std::string& address, int port,
	                      int timeout = 3);
	void subscribe(const ClientObserver& observer);
	ResultType send_msg(const char* msg, size_t size);
    ResultType close();

private:
	void initialize_socket();
	void set_address(const std::string& address, int port);
	void publish_server_msg(const char* msg, size_t msg_size);
	void publish_server_disconnected(const ResultType& ret);
    void recv_server();
    /**
     * @brief 开启线程工作
     */
    void start_worker();
    void terminate_worker();

private:
	static constexpr std::size_t BUFFER_SIZE{2048 * 1024}; // 缓冲区 2M
	FileDescriptor sock_fd_;
	std::atomic_bool is_connected_{false};
	std::atomic_bool is_closed_{true};
	struct sockaddr_in server_; // 服务器
	Buffer<BUFFER_SIZE>
	    buffer_; // 发送数据缓冲区  当前发送不了 等待下次连接时再次发送
	std::vector<ClientObserver> subscibers_; // 订阅
	std::unique_ptr<std::thread> worker_ptr{nullptr};
	std::mutex sub_mutex;
    std::mutex buffer_mutex;
};

#endif // TCPCLIENT_H
