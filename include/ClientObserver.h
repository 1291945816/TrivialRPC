/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/03/20 10:34:12
* @version: 1.0
* @description: 
********************************************************************************/ 
#ifndef CLIENTOBSERVER_H
#define CLIENTOBSERVER_H
#include "ResultType.h"
#include <iostream>


struct ClientObserver{
    std::function<void(const char * msg, size_t size)> incoming_packet_handler = nullptr;
    std::function<void(const ResultType & ret)> disconnection_handler = nullptr;
};


#endif // CLIENTOBSERVER_H
