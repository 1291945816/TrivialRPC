/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/03/14 09:53:15
* @version: 1.0
* @description: 
********************************************************************************/ 
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

#include "Client.h"
#include "FileDescriptor.h"
#include "ResultType.h"
#include "ServerObserver.h"





class TcpServer
{
public: 
    TcpServer();
    
    ~TcpServer();

    ResultType start(int port, int max_num_of_clients = 5,bool remove_dead_client_auto = true);



    /**
     * @brief 初始化 创建socket
     */
    void initialize_socket();

    /**
     * @brief 绑定对应的端口
     * @param port 端口地址
     */
    void bind_addres(int port);

    /**
     * @brief 配置监听的信息
     * @param max_num_of_clients 最大的客户端监听数目
     */
    void listen_client(int max_num_of_clients);

    /**
     * @brief 等待客户端的连接
     * 
     * @param timeout 
     * @return std::string 
     */
    std::string accept_client(uint timeout);

    /**
     * @brief 向服务器添加对应ip的订阅者 即针对某一客户端做处理
     *  但是这个订阅者不是唯一的
     * @param observer 
     */
    void subscribe(const ServerObserver& observer){
        std::lock_guard<std::mutex> lock(subscribers_mtx);
        subscribers_.push_back(observer);
    }
    /**
     * @brief 发送消息到所有的客户端
     * 
     * @param msg 
     * @param size 
     * @return ResultType 
     */
    ResultType send_to_all_clients(const char * msg, size_t size);
    /**
     * @brief 发送消息到指定IP的客户端
     * 
     * @param clientIP 
     * @param msg 
     * @param size 
     * @return ResultType 
     */
    ResultType send_to_client(const std::string & client_ip, const char * msg, size_t size);

    /**
     * @brief 关闭服务器
     * 
     * @return ResultType 
     */
    ResultType close();

    void printClients();



private:
    /**
     * @brief 
     * 
     * @param timeout 设定超时时间
     * @return ResultType 
     */
    ResultType wait_for_client(uint32_t timeout);

    /**
     * @brief 客户端发来消息时 处理对应的消息
     * 
     * @param client 
     * @param msg 
     * @param msg_size 
     */
    void publish_client_msg(const Client& client,const char * msg, size_t msg_size);
    
    /**
     * @brief 客户端失去链接时，处理对应的消息
     * 
     * @param client 
     * @param msg 
     * @param msg_size 
     */
    void publish_client_disconnected(const std::string& client_ip,const std::string& msg);

    /**
     * @brief 负责处理来自客户端的情况
     * 
     * @param client 
     * @param event 
     * @param msg 
     */
    void client_event_handler(const Client& client,ClientEvent event,const std::string& msg);

    /**
     * @brief 发送消息给指定的客户端
     * 
     * @param client 
     * @param msg 
     * @param size 
     * @return ResultType 
     */
    static ResultType send_to_client(const Client & client, const char * msg, size_t size);

    /**
     * @brief 终止正在运行的后台线程
     * 
     */
    void terminate_dead_clients_remover();

    /**
     * @brief 移动失去链接的线程 
     *
     */
    void remove_dead_clients();
    








private:
    struct sockaddr_in server_addr_; // 服务器端socket地址
    struct sockaddr_in client_addr_; // 客户端socket地址


    FileDescriptor sock_fd_; // scoket 句柄
    fd_set fds_;   // 用于轮询

    std::vector<Client*> clients_; // 客户端集合
    std::vector<ServerObserver> subscribers_;

    std::mutex subscribers_mtx; // 订阅者的互斥 
    std::mutex client_mtx; // 客户端的互斥

    std::unique_ptr<std::thread> _clients_remove_thread; // 负责处理失去连接的客户端
    std::atomic_bool stop_remove_clients_task_;

};




#endif // TCPSERVER_H