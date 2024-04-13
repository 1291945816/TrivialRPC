/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/09 09:37:26
* @version: 1.0
* @description: 
********************************************************************************/ 

#include "rpc/RPCServer.h"
#include <iostream>

int add(int a,int b){
    DEBUG_LOG << a << " " << b << " ";
    return a+b;
}
int sub(int a,int b){
    return a-b;
}


int main(){
    RPCServer server;
    server.registerMethod("add", add);
    server.registerMethod("sub", sub);
    server.registerMethod("hello", [](){
        return std::string("hello");
    });
    server.start(8081);
    server.run();



    return 0;
}