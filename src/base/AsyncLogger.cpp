/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 20:57:02
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "base/AsyncLogger.h"
#include "base/Buffer.h"
#include "cassert"
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <functional>
#include <memory>
#include <stdexcept>

AsyncLogger::AsyncLogger(std::string log_file, int flush_interval)
    : base_name_(log_file)
    , flush_time_(flush_interval)
    , running_state(false)
    , cur_buffer_(new Buffer)
    , next_buffer_(new Buffer) {
	init();
}

void AsyncLogger::init() {
	auto fp_ = std::fopen(base_name_.c_str(), "a");
	if (fp_ == NULL)
		perror("open log file failed !\n");
	fp_ptr = std::unique_ptr<std::FILE,Flcoser>(fp_,Flcoser{});
}

void AsyncLogger::append(const char* msg, int len) {
	std::lock_guard<std::mutex> lock_(mutex_);
	if (cur_buffer_->avail() > len) {
		cur_buffer_->append(msg, len);
	} else {
		dirty_buffer_.push_back(std::move(cur_buffer_));
		if (next_buffer_)
			cur_buffer_ = std::move(next_buffer_);
		else
			cur_buffer_.reset(new Buffer); // 多余的备份缓冲区已经使用完了
			                               // 还没有来得及刷新到文件中
		cur_buffer_->append(msg, len);
		cond_.notify_one();
	}
}

void AsyncLogger::append_log_file() {
	std::vector<BufferPtr> to_file_buffers;
	BufferPtr to_file_buffer(new Buffer);
	BufferPtr to_file_buffer_next(new Buffer);
	while (running_state) {

		// 迁移缓冲区的数据
		{
			std::unique_lock<std::mutex> lock_(mutex_);
			assert(cur_buffer_);
			if (cur_buffer_->avail() == Buffer::BUFFER_MAX_SIZE)
				cond_.wait_for(lock_, std::chrono::seconds(flush_time_));

			// 交换
			dirty_buffer_.push_back(std::move(cur_buffer_));
			cur_buffer_ = std::move(to_file_buffer);
			to_file_buffers.swap(dirty_buffer_);
			if (!next_buffer_)
				next_buffer_ = std::move(to_file_buffer_next);
		}
		assert(fp_ptr);
		for (auto& buf : to_file_buffers) {
			int write_len = buf->size();
			auto ret = std::fwrite(buf->data(), 1, write_len, fp_ptr.get());
			if (ret != write_len)
				perror("AsyncLogger::append_log_file() fwrite failed !\n");
		}
		fflush(fp_ptr.get());
		if (to_file_buffers.size() > 2) {
			to_file_buffers.resize(2);
		}
		if (!to_file_buffer) {
			assert(!to_file_buffers.empty());
			to_file_buffer = std::move(to_file_buffers.back());
			to_file_buffers.pop_back();
			to_file_buffer->clear();
		}
		if (!to_file_buffer_next) {
			assert(!to_file_buffers.empty());
			to_file_buffer_next = std::move(to_file_buffers.back());
			to_file_buffers.pop_back();
			to_file_buffer_next->clear();
		}
		to_file_buffers.clear();
	}
	fflush(fp_ptr.get());
}

void AsyncLogger::run() {
	running_state = true;
	worker_thread_ = std::make_unique<std::thread>(
	    std::bind(&AsyncLogger::append_log_file, this));
}
void AsyncLogger::terminate() {
	if (running_state) {
		running_state = false;
		cond_.notify_all();
		worker_thread_->join();
	}
}

AsyncLogger::~AsyncLogger() {
	terminate();
}