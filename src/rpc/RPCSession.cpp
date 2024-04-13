/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/08 08:13:14
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "rpc/RPCSession.h"
#include "base/Logger.h"
#include <bits/types/struct_iovec.h>
#include <sys/socket.h>
#include <vector>

ssize_t RPCSession::read(void* buffer, size_t length) {
	if (!client_ || !client_->is_connected())
		return -1;
	return client_->recv(buffer, length);
}

ssize_t RPCSession::read(ByteArray::ptr buffer, size_t length) {
	if (!client_ || !client_->is_connected())
		return -1;
	std::vector<iovec> iovs;
	buffer->getWriteBuffers(iovs, length);
	auto n = client_->recv(&iovs[0], iovs.size());
	if (n > 0) {
		buffer->setPosition(buffer->getPosition() + n); // 移动操作的位置
	}
	return n;
}

ssize_t RPCSession::write(const void* buffer, size_t length) {
	if (!client_ || !client_->is_connected())
		return -1;
	return client_->send(buffer, length);
}
ssize_t RPCSession::write(ByteArray::ptr buffer, size_t length) {
	if (!client_ || !client_->is_connected())
		return -1;
	std::vector<iovec> iovs;
	buffer->getReadBuffers(iovs, length);
	ssize_t n = client_->send(&iovs[0], iovs.size());
	if (n > 0) {
		buffer->setPosition(buffer->getPosition() + n);
	}
	return n;
}

ssize_t RPCSession::readFixSize(void* buffer, size_t length) {
	size_t offset = 0;
	size_t left = length;
	while (left > 0) {
		ssize_t n = read((char*)buffer + offset, left);
		if (n <= 0) {
            client_->set_connected(false);
			return n;
		}
		offset += n;
		left -= n;
	}
	return length;
}

ssize_t RPCSession::readFixSize(ByteArray::ptr buffer, size_t length) {
	size_t left = length;
	while (left > 0) {
		ssize_t n = read(buffer, left);
		if (n <= 0) {
            client_->set_connected(false);
			return n;
		}
		left -= n;
	}
	return length;
}

ssize_t RPCSession::writeFixSize(const void* buffer, size_t length) {
	size_t offset = 0;
	size_t left = length;
	while (left > 0) {
		ssize_t n = write((const char*)buffer + offset, left);
		if (n <= 0) {
            client_->set_connected(false);
			return n;
		}
		offset += n;
		left -= n;
	}
	return length;
}

ssize_t RPCSession::writeFixSize(ByteArray::ptr buffer, size_t length) {
	size_t offset = 0;
	size_t left = length;
	while (left > 0) {
		ssize_t n = write(buffer, left);
		if (n <= 0) {
            client_->set_connected(false);
			return n;
		}
		offset += n;
		left -= n;
	}
	return length;
}

Protocol::ptr RPCSession::recvProtocol() {
	Protocol::ptr proto = std::make_shared<Protocol>();
	ByteArray::ptr byteArray = std::make_shared<ByteArray>();
    
	if (readFixSize(byteArray, proto->BASE_LENGTH) <= 0) {
        
		return nullptr;
	}

	byteArray->setPosition(0);
    proto->decodeMeta(byteArray);

	if (proto->getMagic() != Protocol::MAGIC) {
        ERROR_LOG << "Magic not match!";
		return nullptr;
	}

	if (!proto->getContentLength()) {
		return proto;
	}

	std::string buff;
	buff.resize(proto->getContentLength());

	if (readFixSize(&buff[0], proto->getContentLength()) <= 0) {
        ERROR_LOG << "have not content data";
		return nullptr;
	}
	proto->setContent(buff);
	return proto;
}

ssize_t RPCSession::sendProtocol(Protocol::ptr proto) {
    ByteArray::ptr byteArray = proto->encode();
    return writeFixSize(byteArray, byteArray->getReadSize());
}