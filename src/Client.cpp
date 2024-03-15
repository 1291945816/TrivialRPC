/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/03/14 10:14:10
* @version: 1.0
* @description: 
********************************************************************************/ 

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <iostream>


#include "Client.h"
#include "common.h"


Client::Client(int file_desc){
    sock_fd_.set(file_desc);
    set_connected(false);
}

void Client::publishEvent(ClientEvent clientEvent, const std::string &msg){
    handler_callback_(*this,clientEvent,msg);
}

void Client::startListen(){
    set_connected(true);
    receive_thread_ = std::make_unique<std::thread>(&Client::receiveTask,this); // 负责处理后台任务  这里可以切入线程池
}

void Client::send(const char * msg,size_t msgSize) const{
    const auto ret = ::send(sock_fd_.get(), msg, msgSize, 0);
    if(ret < 0){
        throw std::runtime_error(strerror(errno));
    }
    // 处理 发送的消息 < 实际长度
    if(ret < msgSize){
        char err_msg[100];
        sprintf(err_msg, "Only %lu bytes out of %lu was sent to client", ret,msgSize);
        throw std::runtime_error(err_msg);
    }
}

void Client::receiveTask(){
    while (is_connected()) {
        const auto ret = fd_wait::wait_for(sock_fd_);
        
        if(ret == fd_wait::Result::FAILURE)
            throw std::runtime_error(strerror(errno));
        else if(ret == fd_wait::Result::TIMEOUT)
            continue;
        
        char rec_buf[MAX_PACKET_SIZE]={'\0'}; 
        auto numofBytes_rec = recv(sock_fd_.get(), &rec_buf, MAX_PACKET_SIZE, 0);
        
        if (numofBytes_rec < 1) {
            std::string disconnect_msg;
            // =0  客户端已经关闭了连接
            if(numofBytes_rec == 0){
                disconnect_msg = "Client has closed connection!";
            }else { // -1 发生错误
                disconnect_msg = strerror(errno);
            }
            set_connected(false);
            publishEvent(ClientEvent::DISCONNECTED,disconnect_msg);
            return;
        }
        else {
            publishEvent(ClientEvent::INCOMING_MSG,rec_buf);
        }
    
    }

}


void Client::terminateReceiveThread(){
    set_connected(false);

    if(receive_thread_){
        receive_thread_->join();
        receive_thread_ = nullptr;
    }

}

void Client::close(){
    terminateReceiveThread();
    const auto ret = ::close(sock_fd_.get());
    if(ret == -1)
        throw std::runtime_error(strerror(errno));
}

void Client::print() const{
    const std::string connected = is_connected() ? "True" : "False";
    std::cout << "-----------------\n" <<
            "IP address: " << get_ip() << std::endl <<
            "Connected?: " << connected << std::endl <<
            "Socket FD: " << sock_fd_.get() << std::endl;
}