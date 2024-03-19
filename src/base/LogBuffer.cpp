/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 15:48:54
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "base/LogBuffer.h"
#include <cstring>
#include <iostream>

int LogBuffer::avail() const { return BUFFER_MAX_SIZE - cur_index_; }
const char* LogBuffer::data() const { return buffer_; }

void LogBuffer::clear(){
	memset(buffer_, '\0', BUFFER_MAX_SIZE);
	cur_index_ = 0;
}

bool LogBuffer::append(const char* str, int len) {
	if (len > avail())
		return false;
	memcpy(&buffer_[cur_index_], str, len);
	cur_index_ += len;
	return true;
}

