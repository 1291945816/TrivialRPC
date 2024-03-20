/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/14 09:53:48
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

/**
 * @brief 文件描述符
 */
#include <functional>
class FileDescriptor {
public:
	FileDescriptor() = default;
	FileDescriptor(int fd)
	    : sock_fd_(fd) {}
	void set(int fd) { sock_fd_ = fd; }
	int get() const { return sock_fd_; }
	bool operator==(const FileDescriptor& rhs) const {
		return sock_fd_ == rhs.sock_fd_;
	}

private:
	int sock_fd_{-1};
};

template <> struct std::hash<FileDescriptor> {
	std::size_t operator()(const FileDescriptor& f) const {
		return std::hash<int>{}(f.get());
	}
};

#endif // FILEDESCRIPTOR_H