/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/22 17:07:01
* @version: 1.0
* @description: 
********************************************************************************/ 
#include "base/util.h"
#include "rpc/ZKClient.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <zookeeper/zookeeper.h>

int main()
{
    ZKClient zk;
    zk.start(); // 发起连接
    // zk.create("/rpc-service", "", 0);

    // 保持会话就不会被关闭
    // zk.create("/rpc-service/testaa", "127.0.0.1:8081", 15,ZOO_PERSISTENT_SEQUENTIAL);
    // std::cout << zk.get_data("/rpc-service/provider-") << "\n";
    // sleep(10);
    // std::string persi_service{"/rpc-service"}; 
    // auto ret = zk.get_children(persi_service.c_str());

    std::string ss{"/abc/xx/"};
    auto ret = parse_path(ss);
    for(const auto & cur:ret){
        std::cout << cur << " ";
        
        // std::cout << zk.get_data((persi_service+"/"+cur).c_str());
        
    }





    return 0;
}