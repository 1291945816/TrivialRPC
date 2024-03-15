/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/03/14 09:54:08
* @version: 1.0
* @description: 
********************************************************************************/ 

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <iostream>


#include "TcpServer.h"
#include "Client.h"
#include "ResultType.h"
#include "common.h"

TcpServer::TcpServer(){
    subscribers_.reserve(10);
    clients_.reserve(10);
    stop_remove_clients_task_ = false;
}



void TcpServer::initialize_socket(){
    // PF_INET ipv4 
    sock_fd_.set(socket(PF_INET, SOCK_STREAM, 0));
    const bool socket_failed = (sock_fd_.get() == -1);
    if(socket_failed){
        throw std::runtime_error(strerror(errno));
    }
    // 重用地址 避免等待
    const int option = 1;
    setsockopt(sock_fd_.get(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

void TcpServer::bind_addres(int port){
    bzero(&server_addr_,sizeof(server_addr_)); 
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr_.sin_port = htons(port);

    const auto ret = bind(sock_fd_.get(), (struct sockaddr*)&server_addr_, sizeof(server_addr_));
    if (ret == -1) {
        throw std::runtime_error(strerror(errno));
    }

}

void TcpServer::listen_client(int max_num_of_clients){
    const auto ret = listen(sock_fd_.get(), max_num_of_clients);
    if (ret == -1) {
        throw std::runtime_error(strerror(errno));
    }
}






ResultType TcpServer::wait_for_client(uint32_t timeout){

    if(timeout > 0){
        const auto ret = fd_wait::wait_for(sock_fd_,timeout);
        const bool no_incoming_client = (!FD_ISSET(sock_fd_.get(),&fds_));

        if(ret == fd_wait::Result::FAILURE)
            return ResultType::FAILURE(strerror(errno));
        else if(ret == fd_wait::Result::TIMEOUT)
            return ResultType::FAILURE("Timeout waiting for client!");
        else if(no_incoming_client)
            return ResultType::FAILURE("File descriptor is not set");

    }

    return ResultType::SUCCESS();
}


void TcpServer::publish_client_msg(const Client& client,const char * msg, size_t msg_size){
    std::lock_guard<std::mutex> lock(subscribers_mtx);

    for (const auto& sub : subscribers_) {
        if(sub.wanted_ip == client.get_ip() || sub.wanted_ip.empty()){
            if(sub.incoming_packet_handler_){
                sub.incoming_packet_handler_(client.get_ip(),msg,msg_size);
            }
        }
    
    }
}

void TcpServer::publish_client_disconnected(const std::string& client_ip,const std::string& msg){
    std::lock_guard<std::mutex> lock(subscribers_mtx);
    
    for (const auto & sub:subscribers_) {
        if(sub.wanted_ip == client_ip || sub.wanted_ip.empty()){
            if(sub.disconnection_handler_){
                sub.disconnection_handler_(client_ip,msg);
            }
        }
    }

}

void TcpServer::client_event_handler(const Client& client,ClientEvent event,const std::string& msg){

    switch (event) {
        case ClientEvent::DISCONNECTED:
        {
            publish_client_disconnected(client.get_ip(), msg);
            break;
        }
        case ClientEvent::INCOMING_MSG:
        {
            publish_client_msg(client, msg.c_str(), msg.size());
        }
    }

}


std::string TcpServer::accept_client(uint timeout)
{
    
    const auto ret = wait_for_client(timeout);

    if(!ret.is_successful()){
        throw std::runtime_error(ret.message());
    }
    
    socklen_t socketSize  = sizeof(client_addr_);
    auto client_desc =  accept(sock_fd_.get(), (struct sockaddr*)&client_addr_, &socketSize);
    if(client_desc == -1)
        throw std::runtime_error(strerror(errno));

    auto client = new Client(client_desc); // 新的连接信息
    client->set_ip(inet_ntoa(client_addr_.sin_addr)); // inet_ntoa 不可重入
    using namespace std::placeholders;
    client->set_event_handler(std::bind(&TcpServer::client_event_handler,this,_1,_2,_3));

    client->startListen();
    
    std::lock_guard<std::mutex> lock(client_mtx);
    clients_.push_back(client);
    
    
    return client->get_ip();
}

ResultType TcpServer::send_to_client(const Client & client, const char * msg, size_t size){

    try {
        client.send(msg, size);
    } catch (const std::runtime_error &err) {
        return ResultType::FAILURE(err.what());
    }
    return ResultType::SUCCESS();
}


ResultType TcpServer::send_to_all_clients(const char * msg, size_t size){
    std::lock_guard<std::mutex> lock(client_mtx);

    for (const auto client: clients_) {
        const auto ret = send_to_client(*client,msg,size);
        if(!ret.is_successful()){
            return ret;
        }
    }
    return ResultType::SUCCESS();
}

ResultType TcpServer::send_to_client(const std::string & client_ip, const char * msg, size_t size)
{
    std::lock_guard<std::mutex> lock(client_mtx);

    auto iter = std::find_if(clients_.begin(),clients_.end(),
                [&client_ip](Client * client){ return client->get_ip() == client_ip;});
    
    if (iter == clients_.end()) {
        return ResultType::FAILURE("client not found!");
    }

    return send_to_client(*(*iter),msg,size);

}

ResultType TcpServer::start(int port, int max_num_of_clients, bool remove_dead_client_auto)
{
    if(remove_dead_client_auto)
        _clients_remove_thread =  std::make_unique<std::thread>(&TcpServer::remove_dead_clients,this);
        // _clients_remove_thread = new std::thread();

    try {
        initialize_socket(); // 初始化socket
        bind_addres(port);
        listen_client(max_num_of_clients);
    
    } catch (const std::runtime_error & err) {
        return ResultType::FAILURE(err.what());
    }
    return ResultType::SUCCESS();

}


void TcpServer::terminate_dead_clients_remover(){
    if(_clients_remove_thread){
        stop_remove_clients_task_ = true;
        _clients_remove_thread->join();

        _clients_remove_thread = nullptr;
    }

}

void TcpServer::remove_dead_clients() {
    std::vector<Client *>::iterator removed_client;
    while (!stop_remove_clients_task_) {
        {

            std::lock_guard<std::mutex> lock(client_mtx);
            removed_client = std::find_if(
                clients_.begin(), clients_.end(),
                [](Client *client) { return !client->is_connected(); });

            while (removed_client != clients_.end()) {

                (*removed_client)->close();
                delete *removed_client;
                clients_.erase(removed_client);

                removed_client = std::find_if(
                    clients_.begin(), clients_.end(),
                    [](Client *client) { return !client->is_connected(); });
            }
        }

        sleep(2); // 间隔2s检查一次
    }
}

ResultType TcpServer::close(){
    terminate_dead_clients_remover();//  终止线程的工作
    
    {
        std::lock_guard<std::mutex> lock(client_mtx);
        for (auto client : clients_) {
            try {
                client->close();
            } catch (const std::runtime_error& err) {
                return ResultType::FAILURE(err.what());
            }
        }
        clients_.clear();
    }

    {
        auto ret = ::close(sock_fd_.get());
        if(ret == -1){
            return ResultType::FAILURE(strerror(errno));
        }
    }
    return ResultType::SUCCESS();
}


void TcpServer::printClients() {
    std::lock_guard<std::mutex> lock(client_mtx);
    if (clients_.empty()) {
        std::cout << "no connected clients\n";
    }
    for (const Client *client : clients_) {
        client->print();
    }
}


TcpServer::~TcpServer()
{
    close();
}