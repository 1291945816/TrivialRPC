/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/20 10:42:40
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "net/TcpClient.h"

#include "net/ResultType.h"
#include "base/Logger.h"
#include "net/common.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

TcpClient::TcpClient(ini::IniFile& ini_file){
	ip_ = ini_file["tcp_client"]["server_ip"].as<std::string>();
	port_ = ini_file["tcp_client"]["server_port"].as<int>();

}


void TcpClient::initialize_socket() {
	sock_fd_.set(socket(AF_INET, SOCK_STREAM, 0));
	const bool socket_failed = (sock_fd_.get() == -1);
	if (socket_failed) {
		throw std::runtime_error(strerror(errno));
	}
	// 非阻塞模式
	int old_socket_flag = fcntl(sock_fd_.get(), F_GETFL, 0);
	int new_socket_flag = old_socket_flag | O_NONBLOCK;
	auto ret = fcntl(sock_fd_.get(), F_SETFL, new_socket_flag);
	if (ret == -1)
		throw std::runtime_error(strerror(errno));
}

void TcpClient::set_address(const std::string& address, int port) {
	auto ret = inet_aton(address.c_str(), &server_.sin_addr);
	// 未解析成功 尝试从字符串中进行解析
	if (!ret) {
		struct hostent* host;
		struct in_addr** addr_list;
		if ((host = gethostbyname(address.c_str())) == nullptr) {
			throw std::runtime_error("Failed to resolve hostname");
		}
		addr_list = (struct in_addr**)host->h_addr_list;
		server_.sin_addr = *addr_list[0];
	}
	server_.sin_family = AF_INET; // ipv4 协议
	server_.sin_port = htons(port);
}

ResultType TcpClient::connect_server() {
	try {
		initialize_socket();
		set_address(ip_, port_);
	} catch (const std::runtime_error& error) {
		ERROR_LOG << error.what();
		return ResultType::FAILURE(error.what());
	}

	while (true) {
		// 连接服务器
		auto connect_result = connect(
		    sock_fd_.get(), (struct sockaddr*)&server_, sizeof(server_));
		if (connect_result == 0) {
			INFO_LOG << "connect to server[" << ip_ << ":" << port_
			         << "] successfully.";
			is_connected_ = true;
            is_closed_ = false;
            start_worker();
			return ResultType::SUCCESS();
		} else if (connect_result == -1) {
			if (errno == EINTR) {
				INFO_LOG << "connect to server[" << ip_ << ":" << port_
				         << "] interruptted by signal, try again.";
				continue;
			} else if (errno == EINPROGRESS) {
				// 连接正在重试中
				INFO_LOG << "connect to server[" << ip_ << ":" << port_
				         << "]  try again....";
				break;
			} else {
				return ResultType::FAILURE(strerror(errno));
			}
		}
	}
	// 继续使用select 进行检测 检测可写事件
	auto ret = fd_wait::wait_for_write(sock_fd_.get());
	if (ret != fd_wait::Result::SUCCESS) {

		ERROR_LOG << "connect to server[" << ip_ << ":" << port_
		          << "] error.";        
		return ResultType::FAILURE(strerror(errno));
	}
	int err;
	socklen_t len = static_cast<socklen_t>(sizeof err);
	if (::getsockopt(sock_fd_.get(), SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
		return ResultType::FAILURE(strerror(errno));
	}
	if (err == 0) {
		INFO_LOG << "[select]connect to server[" << ip_ << ":" << port_
		         << "] successfully.";
		is_connected_ = true;
        is_closed_ = false;
        start_worker();
		return ResultType::SUCCESS();
	} else {
		ERROR_LOG << "connect to server[" << ip_ << ":" << port_
		          << "] error.";
		return ResultType::FAILURE(strerror(errno));
	}
}

void TcpClient::subscribe(const ClientObserver& observer) {
	std::lock_guard<std::mutex> lock_(sub_mutex);
	subscibers_.push_back(observer);
}

void TcpClient::publish_server_msg(const char* msg, size_t msg_size) {
	std::lock_guard<std::mutex> lock_(sub_mutex);
	for (const auto& sub : subscibers_) {
		if (sub.incoming_packet_handler)
			sub.incoming_packet_handler(msg, msg_size);
	}
}

void TcpClient::publish_server_disconnected(const ResultType& ret) {
	std::lock_guard<std::mutex> lock_(sub_mutex);
	for (const auto& sub : subscibers_) {
		if (sub.disconnection_handler)
			sub.disconnection_handler(ret);
	}
}

ResultType TcpClient::send_msg(const char* msg, size_t size) {
	if (size == 0) {
		return ResultType::FAILURE("msg size is 0!");
	}
    std::lock_guard<std::mutex> lock_(buffer_mutex);
	if (buffer_.avail() < size) {
        buffer_.clear();
		return ResultType::FAILURE("Buffer data exceeds limit size!");
	}
	buffer_.append(msg, size);
	int sent_byte = 0;
	int ret = 0;
	while (true) {
        
		ret = send(sock_fd_.get(), buffer_.data() + sent_byte,
		           buffer_.size() - sent_byte, 0);
		if (ret == -1) {
			if (errno == EWOULDBLOCK) {  // 缓存数据
                WARNING_LOG << "Client data is not sent completely!";
				auto len = buffer_.size() - sent_byte;
				auto* data = new char[len + 1];
				std::memcpy(data, buffer_.data() + sent_byte,
				            buffer_.size() - sent_byte);
				buffer_.clear();
				buffer_.append(data, len);
				delete[] data;
				break;
			} else if (errno == EINTR) {
				continue;
			} else
				return ResultType::FAILURE("sent failed!");
		} else if (ret == 0)
			return ResultType::FAILURE("sent failed!");
		sent_byte += ret;
		if (sent_byte == buffer_.size()) {
			buffer_.clear();
			break;
		}
		usleep(1);
	}
	return ResultType::SUCCESS();
}

void TcpClient::recv_server(){
    while (is_connected_) {
        // 等待可读事件
        auto ret = fd_wait::wait_for_read(sock_fd_.get());
        
        if(ret == fd_wait::Result::FAILURE){
            throw std::runtime_error(strerror(errno));
        }else if (ret == fd_wait::Result::TIMEOUT) {
            continue;
        }

        char msg[MAX_PACKET_SIZE]; // 4KB的包
        auto nums_of_byte_recv = recv(sock_fd_.get(), &msg, MAX_PACKET_SIZE, 0);
        if(nums_of_byte_recv < 1){
            std::string error_msg;
            if(nums_of_byte_recv == 0){
                error_msg = "Server closed connection";
            }else{
                error_msg = strerror(errno);
            }
            is_connected_ = false;
            publish_server_disconnected(ResultType::FAILURE(error_msg));
            return ;
        }
        else {
            INFO_LOG << "recv from server data size:"<<nums_of_byte_recv;
            publish_server_msg(msg, nums_of_byte_recv);
        }    
    }
}

void TcpClient::start_worker(){
    worker_ptr = std::make_unique<std::thread>(&TcpClient::recv_server,this);
}


void TcpClient::terminate_worker(){
    is_connected_ = false;
    if(worker_ptr && worker_ptr->joinable())
    {
        worker_ptr->join();
        worker_ptr.reset(nullptr);
    }
}

ResultType TcpClient::close(){
    if (is_closed_) {
        return ResultType::FAILURE("client is already closed");
    }
    terminate_worker();
    auto ret = ::close(sock_fd_.get());
    if (ret == -1) {
        return ResultType::FAILURE(strerror(errno)); 
    }
    is_closed_ = true;
    return ResultType::SUCCESS();
}

TcpClient::~TcpClient(){
    close();
}