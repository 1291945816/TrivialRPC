/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/03/14 09:53:48
* @version: 1.0
* @description: 
********************************************************************************/ 
#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

/**
 * @brief 文件描述符
 */
class FileDescriptor{
public:
    void set(int fd){sock_fd_=fd;}
    int get() const {return sock_fd_;}
private:
    int sock_fd_{0};
};





#endif // FILEDESCRIPTOR_H