/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 15:48:54
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "base/Buffer.h"
#include <cstring>
#include <iostream>

int Buffer::avail() const { return BUFFER_MAX_SIZE - cur_index_; }
const char* Buffer::data() const { return buffer_; }

void Buffer::clear(){
	memset(buffer_, '\0', BUFFER_MAX_SIZE);
	cur_index_ = 0;
}

bool Buffer::append(const char* str, int len) {
	if (len > avail())
		return false;
	memcpy(&buffer_[cur_index_], str, len);
	cur_index_ += len;
	return true;
}

