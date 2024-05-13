/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/09 16:33:19
* @version: 1.0
* @description: 
********************************************************************************/ 


#include "rpc/RPCClient.h"


int main(){
    ini::IniFile my_ini;
    my_ini.load("./test.ini");

    RPCClient client(my_ini);
    
    auto ret_ = client.connect_server();
    if (!ret_.is_successful()) {
        std::cout << ret_.message();
    }
    auto ret = client.call<std::string>("hello");
    std::cout <<ret.getCode() << " "<< ret.getMsg() << ": "<< ret.getVal();

    auto ret1 = client.call<std::string>("hello");
    std::cout <<ret1.getCode() << " "<< ret1.getMsg() << ": "<< ret1.getVal();

    auto ret2 = client.call<std::string>("hello");
    std::cout <<ret2.getCode() << " "<< ret2.getMsg() << ": "<< ret2.getVal();




    return 0;
}
