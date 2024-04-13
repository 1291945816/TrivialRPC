/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/09 16:33:19
* @version: 1.0
* @description: 
********************************************************************************/ 

#include "base/Logger.h"
#include "rpc/RPCClient.h"
#include <string>


int main(){
    RPCClient client;
    client.connect_to("127.0.0.1", 8081);
    auto ret = client.call<std::string>("hello");
    std::cout <<ret.getCode() << " "<< ret.getMsg() << ": "<< ret.getVal();




    return 0;
}
