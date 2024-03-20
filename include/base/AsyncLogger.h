/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 20:39:48
 * @version: 1.0
 * @description:
 ********************************************************************************/

#ifndef ASYNCLOGGER_H
#define ASYNCLOGGER_H

#include "Buffer.hpp"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

struct Flcoser{
	void operator()(std::FILE* fp) const noexcept{
		std::fclose(fp);
	}
};

class AsyncLogger {
public:
	AsyncLogger(std::string log_file, int flush_interval = 2);
	void append(const char*, int);
	void run();
	void terminate();
	~AsyncLogger();

private:
	void init();
	void append_log_file();

private:
	using BufferPtr = std::unique_ptr<Buffer<LOG_BUFFER_SIZE>>;
	using File_ptr = std::unique_ptr<std::FILE,Flcoser>;

	BufferPtr cur_buffer_;                       // 当前缓冲区
	BufferPtr next_buffer_;                      // 另一个缓冲区
	std::vector<BufferPtr> dirty_buffer_;        // 已经处理过的缓冲区
	std::unique_ptr<std::thread> worker_thread_; // 工作线程（负责写日志）
	const std::string base_name_;                // 日志文件名称
	int flush_time_;
	File_ptr fp_ptr;
	// 互斥操作的处理
	std::mutex mutex_;
	std::condition_variable_any cond_;
	std::atomic_bool running_state;
};

#endif // ASYNCLOGGER_H
