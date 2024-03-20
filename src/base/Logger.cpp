/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 23:33:07
 * @version: 1.0
 * @description:
 ********************************************************************************/

#include "base/Logger.h"
#include "base/AsyncLogger.h"
#include "base/Buffer.hpp"
#include "base/LogStream.h"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <sys/time.h>
#include <thread>
#include <time.h>

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static std::unique_ptr<AsyncLogger> async_logger_ptr;
static const char* LEVEL_STRING[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
std::string Logger::log_file_name_ = "run_log.log";
constexpr unsigned int PREINFO_LEN = 256;

#if defined(INFO_FLAG)
Logger::LogLevel g_log_level = Logger::INFO;
#else
Logger::LogLevel g_log_level = Logger::DEBUG;
#endif

class Logger::Impl {
public:
	Impl(const char* filename, int line, const char* func, LogLevel level);
	void format_base_info();
	std::string basename_;
	int line_;
	LogLevel level_;
	const char* func_;
	LogStream stream;
};
Logger::Impl::Impl(const char* filename, int line, const char* func,
                   LogLevel level)
    : basename_(filename)
    , line_(line)
    , level_(level)
    , func_(func)
    , stream() {
	format_base_info();
}

void Logger::Impl::format_base_info() {
	struct timeval tv;
	static char str_t[64] = {0};
	gettimeofday(&tv, NULL);
	static time_t lastsec = 0; // 上次的事件
	if (lastsec != tv.tv_sec) {
		struct tm* p_time = localtime(&tv.tv_sec);
		lastsec = tv.tv_sec;
		int k = snprintf(str_t, 64, "%04d-%02d-%02d %02d:%02d:%02d",
		                 p_time->tm_year + 1900, p_time->tm_mon + 1,
		                 p_time->tm_mday, p_time->tm_hour, p_time->tm_min,
		                 p_time->tm_sec);
		str_t[k] = '\0';
	}

	char output[PREINFO_LEN] = {0};
	std::thread::id tid = std::this_thread::get_id();
	uint32_t n = snprintf(
	    output, PREINFO_LEN, "[%s][%s.%03ld][%s:%d %s][pid:%lu] ",
	    LEVEL_STRING[level_], str_t, tv.tv_usec / 1000, basename_.c_str(),
	    line_, func_, std::hash<std::thread::id>()(tid));
	stream << output;
}

Logger::Logger(const char* filename, int line, const char* func, LogLevel level)
    : impl_ptr(std::make_unique<Impl>(filename, line, func, level)) {}

LogStream& Logger::stream() { return impl_ptr->stream; }

void thread_once() {
	async_logger_ptr.reset(new AsyncLogger(Logger::get_log_name()));
	async_logger_ptr->run();
}

void transfer(const char* msg, int len) {
	pthread_once(&once_control_, thread_once);
	async_logger_ptr->append(msg, len);
}

Logger::~Logger() {
	impl_ptr->stream << "\n";
	const Buffer<LOG_BUFFER_SIZE>& buf(stream().buffer());
	transfer(buf.data(), buf.size());
}