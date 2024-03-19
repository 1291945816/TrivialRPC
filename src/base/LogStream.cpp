/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 16:31:31
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "base/LogStream.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

static const char DIGITS[]{"9876543210123456789"};
static const char DIGITS_HEX[]{"0123456789ABCDEF"};
static const char* ZERO = DIGITS + 9;

/**
 * @brief 负责将整数类型转换为字符串类型
 *
 * @tparam T T 必须是整数类型 不能为浮点类型
 * @param buf
 * @param val
 * @return size_t
 */
template <typename T> size_t convert(char* buf, T val) {
	T i = val;
	auto p = buf;
	do {
		int d = static_cast<int>(i % 10);
		i /= 10;
		*p++ = ZERO[d];
	} while (i);
	if (val < 0)
		*p++ = '-';
	*p = '\0';
	std::reverse(buf, p);
	return p - buf;
}
template <typename T> void LogStream::format_integer(T val) {

	if (buffer_.avail() >= K_MAXNUMERIC_SIZE) {
		char buf[K_MAXNUMERIC_SIZE]{'\0'};
		size_t len = convert(buf, val);
		buffer_.append(buf, len);
	}
}

LogStream& LogStream::operator<<(int val) {
	format_integer(val);
	return *this;
}
LogStream& LogStream::operator<<(unsigned int val) {
	format_integer(val);
	return *this;
}

LogStream& LogStream::operator<<(short val) {
	*this << static_cast<int>(val);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short val) {
	*this << static_cast<unsigned int>(val);
	return *this;
}

LogStream& LogStream::operator<<(long val) {
	format_integer(val);
	return *this;
}
LogStream& LogStream::operator<<(unsigned long val) {
	format_integer(val);
	return *this;
}

/**
 * @brief 处理指针地址
 *
 * @param val
 * @return LogStream&
 */
LogStream& LogStream::operator<<(const void* val) {

	auto v = reinterpret_cast<uintptr_t>(val);
	if (buffer_.avail() >= K_MAXNUMERIC_SIZE) {
		char buf[K_MAXNUMERIC_SIZE]{"0x"};
		uintptr_t i = v;
		char* p = buf + 2;

		do {
			int d = static_cast<int>(i % 16);
			i /= 16;
			*p++ = DIGITS_HEX[d];
		} while (i);
		*p = '\0';
		std::reverse(buf + 2, p);
		buffer_.append(buf, p - buf);
	}
	return *this;
}

LogStream& LogStream::operator<<(long long val) {
	format_integer(val);
	return *this;
}
LogStream& LogStream::operator<<(unsigned long long val) {
	format_integer(val);
	return *this;
}

LogStream& LogStream::operator<<(double val) {
	if (buffer_.avail() >= K_MAXNUMERIC_SIZE) {
		char buf[K_MAXNUMERIC_SIZE]{'\0'};
		auto len = snprintf(buf, K_MAXNUMERIC_SIZE, "%.12g", val);
		buffer_.append(buf, len);
	}
	return *this;
}
LogStream& LogStream::operator<<(long double val) {
	if (buffer_.avail() >= K_MAXNUMERIC_SIZE) {
		char buf[K_MAXNUMERIC_SIZE]{'\0'};
		auto len = snprintf(buf, K_MAXNUMERIC_SIZE, "%.12Lg", val);
		buffer_.append(buf, len);
	}
	return *this;
}
LogStream& LogStream::operator<<(float val) {
	*this << static_cast<double>(val);
	return *this;
}