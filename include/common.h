/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/14 10:58:03
 * @version: 1.0
 * @description:
 ********************************************************************************/

#ifndef COMMON_H
#define COMMON_H

#include "FileDescriptor.h"
#include <cstdint>

#define MAX_PACKET_SIZE 4096

namespace fd_wait {
enum class Result {
	FAILURE, // 失败
	TIMEOUT, // 超时
	SUCCESS, // 成功
};

Result wait_for(const FileDescriptor& file_desc, uint32_t timeout_sec = 1);

} // namespace fd_wait

#endif // COMMON_H
