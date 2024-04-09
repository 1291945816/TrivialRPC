/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/27 16:05:13
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef BYTEARRAY_H
#define BYTEARRAY_H
#include "util.h"
#include <cstddef>
#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
class ByteArray {
public:
	using ptr = std::shared_ptr<ByteArray>;
	struct Node {
		/**
		 * @brief 构造指定大小的内存块
		 *
		 * @param s 内存块的字节数
		 */
		Node(size_t s);

		Node();
		~Node();

		char* ptr_;
		Node* next_;
		size_t size_;
	};

	/**
	 * @brief 使用指定长度的内存块构造ByteArray
	 *
	 * @param base_size 内存块大小
	 */
	ByteArray(size_t base_size = 4096);

	~ByteArray();
	template <class T> void writeFint(T value) {
		if (endian_ != std::endian::native) {
			value = byte_swap(value);
		}
		write(&value, sizeof(value));
	}
	/**
	 * @brief
	 * 写入固定长度int8_t类型的数据
	 position_ += sizeof(value)
	 position_ > size_ 则 size_ = position_
	 * @param value
	 */
	void writeFint8(int8_t value);
	/**
	 * @brief
	 * 写入固定长度uint8_t类型的数据
	 * @param value
	 */
	void writeFuint8(uint8_t value);

	/**
	 * @brief
	 * 写入固定长度int16_t类型的数据
	 * @param value
	 */
	void writeFint16(int16_t value);

	/**
	 * @brief
	 * 写入固定长度uint16_t类型的数据
	 * @param value
	 */
	void writeFuint16(uint16_t value);

	/**
	 * @brief 写入固定长度int32_t类型的数据(大端/小端)
	 *
	 * @param value
	 */
	void writeFint32(int32_t value);
	/**
	 * @brief 写入固定长度uint32_t类型的数据(大端/小端)
	 *
	 * @param value
	 */
	void writeFuint32(uint32_t value);

	/**
	 * @brief 写入固定长度int64_t类型的数据(大端/小端)
	 *
	 * @param value
	 */
	void writeFint64(int64_t value);

	/**
	 * @brief 写入固定长度uint64_t类型的数据(大端/小端)
	 *
	 * @param value
	 */
	void writeFuint64(uint64_t value);

	/**
	 * @brief 写入有符号Varint32类型的数据
	 * position_ += 实际占用内存(1 ~ 5)
	 * 如果position_ > size_ 则 size_ = position_
	 * @param value
	 */
	void writeInt32(int32_t value);
	/**
	 * @brief 写入无符号Varint32类型的数据
	 *
	 * @param value
	 */
	void writeUint32(uint32_t value);
	/**
	 * @brief 写入有符号Varint64类型的数据
	 *
	 * @param value
	 */
	void writeInt64(int64_t value);
	/**
	 * @brief 写入无符号Varint64类型的数据
	 *
	 * @param value
	 */
	void writeUint64(uint64_t value);
	/**
	 * @brief 写入float类型的数据
	 *
	 * @param value
	 */
	void writeFloat(float value);
	/**
	 * @brief 写入double类型的数据
	 *
	 * @param value
	 */
	void writeDouble(double value);
	/**
	 * @brief 写入std::string类型的数据,用uint16_t作为长度类型
	 * position_ += 2 + value.size()
	 * @param value
	 */
	void writeStringF16(const std::string& value);
	/**
	 * @brief 写入std::string类型的数据,用uint32_t作为长度类型
	 * position_ += 4 + value.size()
	 * @param value
	 */
	void writeStringF32(const std::string& value);
	/**
	 * @brief 写入std::string类型的数据,用uint64_t作为长度类型
	 * position_ += 4 + value.size()
	 * @param value
	 */
	void writeStringF64(const std::string& value);

	/**
	 * @brief 写入std::string类型的数据,用无符号Varint64作为长度类型
	 * position_ += Varint64长度 + value.size()
	 * @param value
	 */
	void writeStringVint(const std::string& value);
	/**
	 * @brief
	 * 写入std::string类型的数据,无长度
	 *position_ +=  value.size()
	 * @param value
	 */
	void writeStringWithoutLength(const std::string& value);
	/**
	 * @brief 读取int8_t类型的数据
	 * getReadSize() >= sizeof(int8_t)
	 *  position_ += sizeof(int8_t);
	 * @return int8_t
	 */
	int8_t readFint8();
	/**
	 * @brief 读取uint8_t类型的数据
	 * getReadSize() >= sizeof(uint8_t)
	 * position_ += sizeof(uint8_t);
	 * @return uint8_t
	 */
	uint8_t readFuint8();
	/**
	 * @brief 读取int16_t类型的数据
	 *
	 * @return int16_t
	 */
	int16_t readFint16();
	/**
	 * @brief 读取uint16_t类型的数据
	 *
	 * @return uint16_t
	 */
	uint16_t readFuint16();
	/**
	 * @brief 读取int32_t类型的数据
	 *
	 * @return int32_t
	 */
	int32_t readFint32();
	/**
	 * @brief 读取uint32_t类型的数据
	 *
	 * @return uint32_t
	 */
	uint32_t readFuint32();
	/**
	 * @brief 读取int64_t类型的数据
	 *
	 * @return int64_t
	 */
	int64_t readFint64();
	/**
	 * @brief 读取uint64_t类型的数据
	 *
	 * @return uint64_t
	 */
	uint64_t readFuint64();
	/**
	 * @brief 读取有符号Varint32类型的数据
	 *
	 * @return int32_t
	 */
	int32_t readInt32();

	/**
	 * @brief 读取无符号Varint32类型的数据
	 *
	 * @return uint32_t
	 */
	uint32_t readUint32();
	/**
	 * @brief 读取有符号Varint64类型的数据
	 *
	 * @return int64_t
	 */
	int64_t readInt64();
	/**
	 * @brief 读取无符号Varint64类型的数据
	 *
	 * @return uint64_t
	 */
	uint64_t readUint64();
	/**
	 * @brief 读取float类型的数据
	 *
	 * @return float
	 */
	float readFloat();
	/**
	 * @brief 读取double类型的数据
	 *
	 * @return double
	 */
	double readDouble();
	/**
	 * @brief 读取std::string类型的数据,用uint16_t作为长度
	 *
	 * @return std::string
	 */
	std::string readStringF16();
	/**
	 * @brief 读取std::string类型的数据,用uint32_t作为长度
	 *
	 * @return std::string
	 */
	std::string readStringF32();
	/**
	 * @brief 读取std::string类型的数据,用uint64_t作为长度
	 *
	 * @return std::string
	 */
	std::string readStringF64();
	/**
	 * @brief 读取std::string类型的数据,用无符号Varint64作为长度
	 *
	 * @return std::string
	 */
	std::string readStringVint();
	/**
	 * @brief 清空ByteArray
	 *
	 */
	void clear();
	/**
	 * @brief 写入size长度的数据
	 *
	 * @param buf  内存缓存指针
	 * @param size 数据大小
	 */
	void write(const void* buf, size_t size);
	/**
	 * @brief 读取size长度的数据
	 *
	 * @param buf 内存缓存指针
	 * @param size 数据大小
	 */
	void read(void* buf, size_t size);
	/**
	 * @brief 读取size长度的数据
	 *
	 * @param buf 内存缓存指针
	 * @param size 数据大小
	 * @param position 读取开始位置
	 * @exception 如果 (size_ - position) < size 则抛出 std::out_of_range
	 */
	void read(void* buf, size_t size, size_t position) const;
	/**
	 * @brief  返回ByteArray当前位置
	 *
	 * @return size_t
	 */
	size_t getPosition() const { return position_; }
	/**
	 * @brief 设置ByteArray当前位置
	 *
	 * @param v
	 */
	void setPosition(size_t v);
	/**
	 * @brief 把ByteArray的数据写入到文件中
	 *
	 * @param name
	 * @return true
	 * @return false
	 */
	bool writeToFile(const std::string& name) const;
	/**
	 * @brief 从文件中读取数据
	 *
	 * @param name
	 * @return true
	 * @return false
	 */
	bool readFromFile(const std::string& name);
	/**
	 * @brief 返回内存块的大小
	 *
	 * @return size_t
	 */
	size_t getBaseSize() const { return baseSize_; }
	/**
	 * @brief 返回可读取数据大小
	 *
	 * @return size_t
	 */
	size_t getReadSize() const { return size_ - position_; }
	/**
	 * @brief 是否是小端
	 *
	 * @return true
	 * @return false
	 */
	bool isLittleEndian() const;
	/**
	 * @brief 设置是否为小端
	 *
	 * @param val
	 */
	void setIsLittleEndian(bool val);
	/**
	 * @brief 将ByteArray里面的数据[position_, size_)转成std::string
	 *
	 * @return std::string
	 */
	std::string toString() const;
	/**
	 * @brief 将ByteArray里面的数据[position_,
	 * size_)转成16进制的std::string(格式:FF FF FF)
	 *
	 * @return std::string
	 */
	std::string toHexString() const;
	/**
	 * @brief 获取可读取的缓存,保存成iovec数组
	 *
	 * @param buffers buffers 保存可读取数据的iovec数组
	 * @param len len 读取数据的长度,如果len > getReadSize() 则 len =
	 * getReadSize()
	 * @return uint64_t 返回实际数据的长度
	 */
	uint64_t getReadBuffers(std::vector<iovec>& buffers,
	                        uint64_t len = ~0ull) const;
	/**
	 * @brief 获取可读取的缓存,保存成iovec数组,从position位置开始
	 *
	 * @param buffers 保存可读取数据的iovec数组
	 * @param len 读取数据的长度,如果len > getReadSize() 则 len = getReadSize()
	 * @param position
	 * @return uint64_t
	 */
	uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len,
	                        uint64_t position) const;
	/**
	 * @brief 获取可写入的缓存,保存成iovec数组
	 *
	 * @param buffers 保存可写入的内存的iovec数组
	 * @param len 写入的长度
	 * @return 如果(position_ + len) > capacity_ 则
	 * capacity_扩容N个节点以容纳len长度
	 */
	uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);
	/**
	 * @brief 返回数据的长度
	 *
	 * @return size_t
	 */
	size_t getSize() const { return size_; }

private:
    /**
     * @brief 扩容ByteArray,使其可以容纳size个数据(如果原本可以可以容纳,则不扩容)
     * 
     * @param size 
     */
    void addCapacity(size_t size);
    /**
     * @brief 获取当前的可写入容量
     * 
     * @return size_t 
     */
    size_t getCapacity() const { return capacity_ - position_; }

private:
	size_t baseSize_;    // 内存块大小
	size_t position_;    // 当前的操作位置
	size_t capacity_;    // 当前的总容量
	size_t size_;        // 当前数据的大小
	std::endian endian_; // 字节序
	Node* root_;         // 内存块的起始指针
	Node* cur_;          // 当前指针
};

#endif // BYTEARRAY_H