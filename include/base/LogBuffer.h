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
class LogBuffer {
public:
	bool append(const char* str, int len);
	int avail() const;
	const char* data() const;
	void clear();
	int size() const{ return cur_index_;}
    static constexpr size_t BUFFER_MAX_SIZE{2048}; 
private:
	
	char buffer_[BUFFER_MAX_SIZE]{'\0'}; // 缓冲区
	int cur_index_{0};
};

#endif // LOGBUFFER_H