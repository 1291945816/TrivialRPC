/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/04/09 09:37:16
* @version: 1.0
* @description: 
********************************************************************************/ 
#include "rpc/Protocol.h"
#include "base/ByteArray.h"
#include <cassert>
#include <exception>
#include <memory>
#include <string>

int main(){
    Protocol::ptr proto = Protocol::Create(Protocol::MsgType::RPC_METHOD_REQUEST, "Hello");
    auto byte = proto->encode();
    auto str = byte->toString(); // 转为字符r

    ByteArray::ptr b = std::make_shared<ByteArray>();
    b->writeStringWithoutLength(std::string(str.c_str()));
    b->setPosition(0);
    Protocol::ptr p = std::make_shared<Protocol>();
    try {
        p->decode(b);
    } catch (std::exception& e) {
        std::cout << e.what() << " ";
        return 1;
    }
    
    assert(p->getMagic() == proto->getMagic());
    assert(p->getMsgType() == proto->getMsgType());
    assert(p->getContent().length() == proto->getContent().length());
    assert(p->getVersion() == proto->getVersion());


}