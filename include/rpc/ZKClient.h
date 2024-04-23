/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/22 14:55:29
* @version: 1.0
* @description: 
********************************************************************************/ 
#ifndef ZKCLIENT_H
#define ZKCLIENT_H

#include <vector>
#define THREADED
#include <iostream>
#include <zookeeper/zookeeper.h>

class ZKClient{
public:
    ~ZKClient();
    void start();
    /**
     * @brief 创建一个节点
     * 默认是创建一个持久性的节点
     * @param path 
     * @param data 
     * @param data_len 
     * @param state 根据这个值进行考察
     */
    void create(const char *path, const char *data,int data_len,int state = 0);
    std::string get_data(const char *path);
    /**
     * @brief 用于检测节点是否存在
     * @param path 
     * @return true 存在节点
     * @return false 不存在 
     */
    bool check_znode_exists(const char *path);

    /**
     * @brief 获取某路径下的所有子节点
     * @param path 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> get_children(const char *path);

private:
    
    zhandle_t *zhandle_{nullptr}; // 连接的句柄
};







#endif // ZKCLIENT_H