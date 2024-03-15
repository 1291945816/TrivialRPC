/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/03/14 14:46:04
* @version: 1.0
* @description: 
********************************************************************************/ 

#ifndef RESULTTYPE_H
#define RESULTTYPE_H

#include <algorithm>
#include <string>
class ResultType{

public:
    ResultType()=default;
    ResultType(bool success_flag, std::string msg):success_flag_(success_flag),msg_(std::move(msg)){ }
    std::string message() const { return msg_;}
    bool is_successful() const { return success_flag_;}
    static ResultType FAILURE(const std::string& msg) { return ResultType(false,msg); }
    static ResultType SUCCESS(const std::string& msg="") {return ResultType(true,msg); }

private:
    bool success_flag_{false};
    std::string msg_{""};

};






#endif // RESULTTYPE_H
