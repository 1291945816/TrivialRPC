
/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/09 09:37:38
* @version: 1.0
* @description: 
********************************************************************************/ 
#include "base/Logger.h"
#include "rpc/Serializer.h"
#include "rpc/RPCCommon.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>

void test1(){
    Serializer s;
    RPCResult<int> r,t;
    r.setCode(RPC_SUCCESS);
    r.setVal(23);
    s << r; // 序列化
    s.reset();
    s >> t; // 反序列化
    DEBUG_LOG  << t.getCode() <<" "<<t.getMsg() << " " << t.getVal() << " "; 
    DEBUG_LOG << "TEST";
    sleep(1);
}

void test2(){
    Serializer s;
    std::vector<int> vec{1,2,3,4,5,6},tt;
    s << vec;
    s.reset();
    s >> tt;
    tt[0];
}

/**
 * @brief 无法通过测试
 * 
 */
// void test3(){
//     Serializer s;
//     std::tuple<int,double> a{1,1.1},t;
//     s << a;
//     s >> t;

// }

enum class Color:uint8_t{
    RED,
    GREEN,
    YELLOW
};

// 非常简单的一个类型
struct UserDefine{
    int a{};
    char b{};
    std::string c{};
    Color d;
    std::vector<int> e;
    void toString() const{
        std::cout << a << " " << b << " " << c <<" "; 

    }
};
void test10(){
    UserDefine user = UserDefine{.a = 1,.b = '2',.c="3",.d = Color::GREEN,.e ={4,5}};
    Serializer s;
    s << user;
    s.reset();
    user = UserDefine();
    s >> user;
    user.toString();
}



int main(){
    test1();
    test2();
    test10();
    return 0;
}