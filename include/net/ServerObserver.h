/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/14 15:40:46
 * @version: 1.0
 * @description:
 ********************************************************************************/

#ifndef SERVEROBSERVER_H
#define SERVEROBSERVER_H

#include "base/ByteArray.h"
#include <functional>
#include <string>

struct ServerObserver {
	std::string wanted_ip{""}; // "" 代表支持任意的ip地址
	using IncomingPacketHandlerType = std::function<void(
	    const std::string& ip, ByteArray::ptr)>;
	using DisconnectedHandlerType =
	    std::function<void(const std::string& ip, ByteArray::ptr)>;

	IncomingPacketHandlerType
	    incoming_packet_handler_; // 收到来自客户端的消息时
	DisconnectedHandlerType
	    disconnection_handler_; // 处理在客户端失去连接时的消息
};

#endif // SERVEROBSERVER_H
