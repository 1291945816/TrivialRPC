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
#include "base/Logger.h"
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <net/if.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/select.h>
#define MAX_PACKET_SIZE 4096

namespace fd_wait {
enum class Result {
	FAILURE, // 失败
	TIMEOUT, // 超时
	SUCCESS, // 成功
};

Result wait_for_read(const FileDescriptor& file_desc, uint32_t timeout_sec = 3);
Result wait_for_write(const FileDescriptor& file_desc,
                      uint32_t timeout_sec = 3);

} // namespace fd_wait

/**
 * @brief get local ip info form ens33 or eth0.
 *
 * @return std::string
 */
inline std::string get_local_ip() {
	INFO_LOG << "get local ip info form ens33 or eth0. ";
	int sock_fd;
	struct ifconf ifconf;
	// 针对某一网卡设备获取对应的信息
	struct ifreq* ifreq = nullptr; 
	char buf[512]; // 缓冲区
	// 保存了所有的网卡设备的信息
	ifconf.ifc_len = 512; 
	ifconf.ifc_buf = buf;
	// 创建一个socket
	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return std::string{};
	}
	ioctl(sock_fd, SIOCGIFCONF, &ifconf);
	ifreq = (struct ifreq*)ifconf.ifc_buf;
	// 找到eth0=> 网卡的名称的信息
	for (int i = (ifconf.ifc_len / sizeof(struct ifreq)); i > 0; i--) {
		if (ifreq->ifr_flags == AF_INET) {
			if (ifreq->ifr_name == std::string("ens33") ||
			    ifreq->ifr_name == std::string("eth0")) {
				std::cout << inet_ntoa(
				    ((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
			}
			ifreq++;
		}
	}
	return std::string{};
}
#endif // COMMON_H
