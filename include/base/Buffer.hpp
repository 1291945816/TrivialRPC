/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 15:13:09
 * @version: 1.0
 * @description:
 ********************************************************************************/

#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include <cstddef>
#include <string.h>
template <std::size_t SIZE=2048> 
class Buffer {
public:
	bool append(const char* str, int len) {
		if (len > avail())
			return false;
		memcpy(&buffer_[cur_index_], str, len);
		cur_index_ += len;
		return true;
	}
	int avail() const { return BUFFER_MAX_SIZE - cur_index_; }
	const char* data() const { return buffer_; }
	void clear() {
		memset(buffer_, '\0', BUFFER_MAX_SIZE);
		cur_index_ = 0;
	}
	int size() const { return cur_index_; }
	static constexpr size_t BUFFER_MAX_SIZE{SIZE};

private:
	char buffer_[BUFFER_MAX_SIZE]{'\0'}; // 缓冲区
	int cur_index_{0};
};

#define  LOG_BUFFER_SIZE 2048

#endif // LOGBUFFER_H