/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/02 08:56:30
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "base/ByteArray.h"
#include "base/Logger.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
/**
 * @brief 协议规定
    | magic | version | type | sequence id | content length | content byte[]
 */
class Protocol {

public:
	using ptr = std::shared_ptr<Protocol>;
	static constexpr uint8_t MAGIC = 0x09;
	static constexpr uint8_t DEFAULT_VERSION = 0X01;
	static constexpr uint8_t BASE_LENGTH = 11;

	enum class MsgType : uint8_t {
		HEARTBEAT_PACKET, // 心跳包

		RPC_PROVIDER, // 向服务中心声明为provider
		RPC_CONSUMER, // 向服务中心声明为consumer

		RPC_REQUEST,         // 通用请求
		RPC_RESPONSE,        // 通用响应
		RPC_METHOD_REQUEST,  // 请求方法调用
		RPC_METHOD_RESPONSE, // 响应方法调用

		RPC_SERVICE_REGISTER, //  向中心注册服务
		RPC_SERVICE_REGISTER_RESPONSE,

		RPC_SERVICE_DISCOVER, // 向中心请求服务发现
		RPC_SERVICE_DISCOVER_RESPONSE,

	};

	// 构建
	static Protocol::ptr Create(MsgType type, const std::string& content,
	                            uint32_t id = 0) {
		Protocol::ptr proto = std::make_shared<Protocol>();
		proto->setMsgType(type);
		proto->setContent(content);
		proto->setSequenceId(id);
		return proto;
	}
	static Protocol::ptr HeartBeat() {
		static Protocol::ptr Heartbeat =
		    Protocol::Create(Protocol::MsgType::HEARTBEAT_PACKET, "");
		return Heartbeat;
	}

	ByteArray::ptr encodeMeta() {
		ByteArray::ptr bt = std::make_shared<ByteArray>();
		bt->writeFuint8(magic_);
		bt->writeFuint8(version_);
		bt->writeFuint8(type_);
		bt->writeFuint32(sequence_id_);
		bt->writeFuint32(content_.size());
		bt->setPosition(0);
		return bt;
	}

	ByteArray::ptr encode() {
		ByteArray::ptr bt = std::make_shared<ByteArray>();
		bt->writeFuint8(magic_);
		bt->writeFuint8(version_);
		bt->writeFuint8(type_);
		bt->writeFuint32(sequence_id_);
		bt->writeStringF32(content_);
		bt->setPosition(0);
		return bt;
	}
	void decodeMeta(ByteArray::ptr bt) {
		magic_ = bt->readFuint8();
		version_ = bt->readFuint8();
		type_ = bt->readFuint8();
		sequence_id_ = bt->readFuint32();
		content_length_ = bt->readFuint32();
	}
	void decode(ByteArray::ptr bt) {
		magic_ = bt->readFuint8();
		version_ = bt->readFuint8();
		type_ = bt->readFuint8();
		sequence_id_ = bt->readFuint32();
		content_ = bt->readStringF32();
		content_length_ = content_.size();
	}

	/**构造*/
	void setMagic(uint8_t magic) { magic_ = magic; }
	void setVersion(uint8_t version) { version_ = version; }
	void setMsgType(MsgType type) { type_ = static_cast<uint8_t>(type); }
	void setSequenceId(uint32_t id) { sequence_id_ = id; }
	void setContentLength(uint32_t len) { content_length_ = len; }
	void setContent(std::string content) { content_ = std::move(content); }

	/**
	 * 取值
	 */
	uint8_t getMagic() { return magic_; }
	uint8_t getVersion() { return version_; }
	MsgType getMsgType() { return static_cast<MsgType>(type_); }
	uint32_t getSequenceId() { return sequence_id_; }
	uint32_t getContentLength() { return content_length_; }
	const std::string& getContent() { return content_; }

	std::string toString() {
		std::stringstream ss;
		ss << "magic:" << magic_ << ", version:" << version_
		   << ", type:" << type_ << ", id:" << sequence_id_
		   << ", length:" << content_length_ << ", content:" << content_;
		return ss.str();
	}

private:
	uint8_t magic_ = MAGIC;
	uint8_t version_ = DEFAULT_VERSION;
	uint8_t type_ = 0;
	uint32_t sequence_id_ = 0;
	uint32_t content_length_ = 0;
	std::string content_;
};

#endif // PROTOCOL_H