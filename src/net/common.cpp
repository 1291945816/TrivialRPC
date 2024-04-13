/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/14 11:01:08
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "net/common.h"
#include <cstddef>
#include <sys/select.h>

#define SELECT_FAILED -1 // 失败
#define SELECT_TIMEOUT 0 // 超时

namespace fd_wait {

Result wait_for_read(const FileDescriptor& file_desc, uint32_t timeout_sec) {
	struct timeval tv;
	tv.tv_sec = timeout_sec;
	tv.tv_usec = 0;

	fd_set fds;

	// fds 所有位置0
	FD_ZERO(&fds);
	// 设置fdset的位
	FD_SET(file_desc.get(), &fds);

	const auto ret = select(file_desc.get() + 1, &fds, nullptr, nullptr, &tv);
	if (ret == SELECT_FAILED)
		return Result::FAILURE;
	else if (ret == SELECT_TIMEOUT)
		return Result::TIMEOUT;
	else
		return Result::SUCCESS;
}
Result wait_for_write(const FileDescriptor& file_desc, uint32_t timeout_sec) {
	struct timeval tv;
	tv.tv_sec = timeout_sec;
	tv.tv_usec = 0;

	fd_set writeset;;

	// fds 所有位置0
	FD_ZERO(&writeset);
	// 设置fdset的位
	FD_SET(file_desc.get(), &writeset);
	const auto ret = select(file_desc.get() + 1, nullptr, &writeset, nullptr, &tv);
	if (ret == SELECT_FAILED)
		return Result::FAILURE;
	else if (ret == SELECT_TIMEOUT)
		return Result::TIMEOUT;
	else
		return Result::SUCCESS;
}

} // namespace fd_wait
