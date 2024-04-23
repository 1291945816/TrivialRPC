/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/09 09:37:26
* @version: 1.0
* @description: 
********************************************************************************/ 

#include "rpc/RPCServer.h"
#include "inicpp.h"
#include <iostream>

int add(int a,int b){
    DEBUG_LOG << a << " " << b << " ";
    return a+b;
}
int sub(int a,int b){
    return a-b;
}


int main(){
    ini::IniFile my_ini;
    my_ini.load("./test.ini");
    RPCServer server(my_ini);
    server.registerService("/rpc-service");
    server.registerMethod("add", add);
    server.registerMethod("sub", sub);
    server.registerMethod("hello", [](){
        return std::string("hello");
    });
    server.run();




    return 0;
}