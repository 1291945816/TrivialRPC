/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/14 09:54:08
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "TcpServer.h"
#include "Client.h"
#include "FileDescriptor.h"
#include "ResultType.h"
#include "base/Logger.h"
#include "common.h"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <utility>

TcpServer::TcpServer() {
	subscribers_.reserve(10);
	clients_idx_.reserve(10);
	stop_remove_clients_task_ = false;
}

void TcpServer::initialize_socket() {
	// PF_INET ipv4
	sock_fd_.set(socket(PF_INET, SOCK_STREAM, 0));
	const bool socket_failed = (sock_fd_.get() == -1);
	if (socket_failed) {
		throw std::runtime_error(strerror(errno));
	}
	// 重用地址 避免等待
	const int option = 1;
	setsockopt(sock_fd_.get(), SOL_SOCKET, SO_REUSEADDR, &option,
	           sizeof(option));
}

void TcpServer::bind_addres(int port) {
	bzero(&server_addr_, sizeof(server_addr_));
	server_addr_.sin_family = AF_INET;
	server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr_.sin_port = htons(port);

	const auto ret = bind(sock_fd_.get(), (struct sockaddr*)&server_addr_,
	                      sizeof(server_addr_));
	if (ret == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void TcpServer::listen_client(int max_num_of_clients) {
	const auto ret = listen(sock_fd_.get(), max_num_of_clients);
	if (ret == -1) {
		throw std::runtime_error(strerror(errno));
	}
}

void TcpServer::init_epoll(int size) {
	epoll_fd = epoll_create(size);
	if (epoll_fd == -1)
		throw std::runtime_error(strerror(errno));
}

void TcpServer::publish_client_msg(const Client& client, const char* msg,
                                   size_t msg_size) {
	std::lock_guard<std::mutex> lock(subscribers_mtx);

	for (const auto& sub : subscribers_) {
		if (sub.wanted_ip == client.get_ip() || sub.wanted_ip.empty()) {
			if (sub.incoming_packet_handler_) {
				sub.incoming_packet_handler_(client.get_ip(), msg, msg_size);
			}
		}
	}
}

void TcpServer::publish_client_disconnected(const std::string& client_ip,
                                            const std::string& msg) {
	std::lock_guard<std::mutex> lock(subscribers_mtx);

	for (const auto& sub : subscribers_) {
		if (sub.wanted_ip == client_ip || sub.wanted_ip.empty()) {
			if (sub.disconnection_handler_) {
				sub.disconnection_handler_(client_ip, msg);
			}
		}
	}
}

void TcpServer::client_event_handler(const Client& client, ClientEvent event,
                                     const std::string& msg) {

	switch (event) {
	case ClientEvent::DISCONNECTED: {
		publish_client_disconnected(client.get_ip(), msg);
		break;
	}
	case ClientEvent::INCOMING_MSG: {
		publish_client_msg(client, msg.c_str(), msg.size());
	}
	}
}

void TcpServer::run() {
	INFO_LOG << "Server::run";
	while (true) {
		auto ret = epoll_wait(epoll_fd, events_, MAX_EVENT_NUMBER, -1);
		if (ret == -1)
			throw std::runtime_error(strerror(errno));
		else
			handle_et_event(ret);
	}
}

std::string TcpServer::accept_client() {

	socklen_t socketSize = sizeof(client_addr_);
	auto client_desc =
	    accept(sock_fd_.get(), (struct sockaddr*)&client_addr_, &socketSize);
	if (client_desc == -1)
		throw std::runtime_error(strerror(errno));

	auto client = new Client(client_desc); // 新的连接信息
	FileDescriptor client_file_desc(client_desc);
	client->set_ip(inet_ntoa(client_addr_.sin_addr)); // inet_ntoa 不可重入
	using namespace std::placeholders;
	client->set_event_handler(
	    std::bind(&TcpServer::client_event_handler, this, _1, _2, _3));
	add_fd(client_file_desc);

	std::lock_guard<std::mutex> lock(client_mtx);
	clients_idx_.emplace(client_file_desc, client);

	return client->get_ip();
}

ResultType TcpServer::send_to_client(const Client& client, const char* msg,
                                     size_t size) {

	try {
		client.send(msg, size);
	} catch (const std::runtime_error& err) {
		return ResultType::FAILURE(err.what());
	}
	return ResultType::SUCCESS();
}

ResultType TcpServer::send_to_all_clients(const char* msg, size_t size) {
	std::lock_guard<std::mutex> lock(client_mtx);

	for (const auto [file_desc, client] : clients_idx_) {
		const auto ret = send_to_client(*client, msg, size);
		if (!ret.is_successful()) {
			return ret;
		}
	}
	return ResultType::SUCCESS();
}

ResultType TcpServer::send_to_client(const std::string& client_ip,
                                     const char* msg, size_t size) {
	std::lock_guard<std::mutex> lock(client_mtx);

	auto iter =
	    std::find_if(clients_idx_.begin(), clients_idx_.end(),
	                 [&client_ip](std::pair<FileDescriptor, Client*> client) {
		                 return (client.second)->get_ip() == client_ip;
	                 });

	if (iter == clients_idx_.end()) {
		return ResultType::FAILURE("client not found!");
	}

	return send_to_client(*(iter->second), msg, size);
}

ResultType TcpServer::start(int port, int max_num_of_clients,
                            bool remove_dead_client_auto) {
	if (remove_dead_client_auto)
		_clients_remove_thread = std::make_unique<std::thread>(
		    &TcpServer::remove_dead_clients, this);

	try {
		initialize_socket(); // 初始化socket
		bind_addres(port);
		listen_client(max_num_of_clients);
		init_epoll(10);
	} catch (const std::runtime_error& err) {
		return ResultType::FAILURE(err.what());
	}
	add_fd(sock_fd_, false);
	// 其他初始化工作
	threadpool.reset(new putils::ThreadPool{}); // 配置线程数目
	INFO_LOG << "Server::start in " << port;
	return ResultType::SUCCESS();
}

void TcpServer::terminate_dead_clients_remover() {
	if (_clients_remove_thread && _clients_remove_thread->joinable()) {
		stop_remove_clients_task_ = true;
		_clients_remove_thread->join();

		_clients_remove_thread = nullptr;
	}
}

void TcpServer::remove_dead_clients() {
	std::unordered_map<FileDescriptor, Client*>::iterator removed_client;
	while (!stop_remove_clients_task_) {
		{
			std::lock_guard<std::mutex> lock(client_mtx);
			removed_client = std::find_if(
			    clients_idx_.begin(), clients_idx_.end(),
			    [](const std::pair<FileDescriptor, Client*>& client) {
				    return !(client.second)->is_connected();
			    });

			while (removed_client != clients_idx_.end()) {

				(removed_client->second)->close();
				client_write_mtx_.erase(removed_client->first);
				delete removed_client->second;
				clients_idx_.erase(removed_client);
				

				removed_client = std::find_if(
				    clients_idx_.begin(), clients_idx_.end(),
				    [](const std::pair<FileDescriptor, Client*>& client) {
					    return !(client.second)->is_connected();
				    });
			}
		}

		sleep(2); // 间隔2s检查一次
	}
}

ResultType TcpServer::close() {
	terminate_dead_clients_remover(); //  终止线程的工作

	{
		std::lock_guard<std::mutex> lock(client_mtx);
		for (const auto [file_desc, client] : clients_idx_) {
			try {
				client->close();
			} catch (const std::runtime_error& err) {
				return ResultType::FAILURE(err.what());
			}
		}
		clients_idx_.clear();
	}

	{
		auto ret = ::close(sock_fd_.get());
		if (ret == -1) {
			return ResultType::FAILURE(strerror(errno));
		}
	}
	return ResultType::SUCCESS();
}

void TcpServer::printClients() {
	std::lock_guard<std::mutex> lock(client_mtx);

	if (clients_idx_.empty()) {
		DEBUG_LOG << "no connected clients";
	}
	for (const auto [file_desc, client] : clients_idx_) {
		client->print();
	}
}

TcpServer::~TcpServer() { close(); }

int TcpServer::set_nonblock(FileDescriptor file_desc) {
	int old_option = fcntl(file_desc.get(), F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(file_desc.get(), F_SETFL, new_option);
	return old_option;
}

void TcpServer::add_fd(FileDescriptor file_desc, bool oneshot) {
	epoll_event event;
	event.data.fd = file_desc.get();
	event.events = EPOLLIN | EPOLLET; // 采用边缘触发模式
	// 一次只能有一个线程处理这个
	if (oneshot) {
		event.events |= EPOLLONESHOT;
	}
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, file_desc.get(), &event);
	set_nonblock(file_desc);
}

void TcpServer::reset_oneshot(FileDescriptor file_desc) {
	epoll_event event;
	event.data.fd = file_desc.get();
	event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, file_desc.get(), &event);
}

void TcpServer::handle_et_event(int number) {
	for (int i = 0; i < number; i++) {
		auto socket_fd = events_[i].data.fd;
		if (socket_fd == sock_fd_.get()) { // 客户端连接
			auto clientIP = accept_client();
			printClients();

		} else if (events_[i].events & EPOLLIN) { // 可读事件
			FileDescriptor file_desc(socket_fd);
			auto client = clients_idx_[file_desc];
			if(client_write_mtx_.count(file_desc) == 0)
				client_write_mtx_.emplace(file_desc,std::mutex()); // 构造

			// 对客户端处理数据
			threadpool->submit([this, &client, &file_desc]() {
				auto ret = client->receive_data();
				if (ret) {
					reset_oneshot(file_desc);
				}
			});
		}
	}
}
