/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/07 22:44:57
* @version: 1.0
* @description: 
********************************************************************************/ 

/**
 * @brief 负责处理服务端之间的协议
 */
#include "net/Client.h"
#include "base/ByteArray.h"
#include "Protocol.h"
#include <memory>
class RPCSession{
    
public:
    using ptr = std::shared_ptr<RPCSession>;
    bool is_connected(){return client_ && client_->is_connected();}
    RPCSession(std::shared_ptr<Client> client):client_(client){}
    // 处理协议
    Protocol::ptr recvProtocol();

    // 发送协议
    ssize_t sendProtocol(Protocol::ptr proto);
private:
    // 读取数据
     ssize_t read(void* buffer, size_t length);
     ssize_t read(ByteArray::ptr buffer, size_t length);

     //写数据
    ssize_t write(const void* buffer, size_t length);
    ssize_t write(ByteArray::ptr buffer, size_t length) ;

    ssize_t readFixSize(void* buffer, size_t length);
    ssize_t readFixSize(ByteArray::ptr buffer, size_t length);

    ssize_t writeFixSize(const void* buffer, size_t length);
    ssize_t writeFixSize(ByteArray::ptr buffer, size_t length);

private:
    std::shared_ptr<Client> client_; // 保存client的信息
};
