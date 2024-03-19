/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/03/18 23:11:30
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef LOGGER_H
#define LOGGER_H

#include "base/LogStream.h"
#include <memory>
#include <string>
class Logger {
public:
	enum LogLevel {
		DEBUG = 0,
		INFO,
		WARNING,
		ERROR,
	};
	Logger(const char* filename, int line, const char* func, LogLevel level);
	~Logger();
	LogStream& stream();
	static void set_log_name(std::string log_file_name) {
		log_file_name_ = std::move(log_file_name);
	}
	static std::string get_log_name() { return log_file_name_; }

private:
	class Impl;
	std::unique_ptr<Impl> impl_ptr;
	static std::string log_file_name_;
};

extern Logger::LogLevel g_log_level;

#define DEBUG_LOG                               \
	if (g_log_level <= Logger::LogLevel::DEBUG) \
	Logger(__FILE__, __LINE__, __FUNCTION__, Logger::DEBUG).stream()

#define INFO_LOG                               \
	if (g_log_level <= Logger::LogLevel::INFO) \
	Logger(__FILE__, __LINE__, __FUNCTION__, Logger::INFO).stream()

#define WARNING_LOG \
	Logger(__FILE__, __LINE__, __FUNCTION__, Logger::WARNING).stream()

#define ERROR_LOG \
	Logger(__FILE__, __LINE__, __FUNCTION__, Logger::ERROR).stream()

#endif // LOGGER_H
