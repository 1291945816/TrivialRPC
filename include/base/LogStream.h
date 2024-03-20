/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 15:59:42
 * @version: 1.0
 * @description:
 ********************************************************************************/

#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include "Buffer.h"
#include <cstring>
#include <iostream>
#include <string>
class LogStream {
public:
	LogStream& operator<<(bool v) {
		buffer_.append(v ? "T" : "F", 1);
		return *this;
	}
	LogStream& operator<<(char v) {
		buffer_.append(&v, 1);
		return *this;
	}
	LogStream& operator<<(const char* str) {
		if (str)
			buffer_.append(str, strlen(str));
		else
			buffer_.append("NULL", 4);
		return *this;
	}
	LogStream& operator<<(const std::string& str) {
		buffer_.append(str.c_str(), str.size());
		return *this;
	}
	LogStream& operator<<(const unsigned char* val) {
		*this << reinterpret_cast<const char*>(val);
		return *this;
	}

	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);
	LogStream& operator<<(const void*);
	LogStream& operator<<(float);
	LogStream& operator<<(double);
	LogStream& operator<<(long double);

	bool append(const char* str, int len) { return buffer_.append(str, len); }
	const Buffer& buffer() const { return buffer_; }

private:
	Buffer buffer_;
	static constexpr int K_MAXNUMERIC_SIZE{32};
	/**
	 * @brief 用于格式化整数
	 *
	 * @tparam T
	 */
	template <typename T> void format_integer(T);
};

#endif // LOGSTREAM_H