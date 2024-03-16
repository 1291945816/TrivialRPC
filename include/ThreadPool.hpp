/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2023/11/27 10:44
 * @version: 1.0
 * @description:
 ********************************************************************************/
#pragma one
#ifndef THREADPOOL_THREADPOOL_HPP
#define THREADPOOL_THREADPOOL_HPP

#include "ThreadSafeQueue.hpp"
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

namespace putils {

class ThreadPool {
public:
	using TaskType = std::function<void()>;
	explicit ThreadPool(
	    std::size_t thread_size = std::thread::hardware_concurrency()) {

		for (std::size_t i = 0; i < thread_size; ++i) {
			workers_.emplace_back([this]() {
				while (true) {
					TaskType task;
					if (!tasks_.waitAndPop(task))
						return;
					task();
				}
			});
		}
	}
#if __cplusplus == 201103L
	template <typename F, typename... Args>
	auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
		std::function<decltype(f(args...))()> func =
		    std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		// 用异步操作封装
		auto p_task =
		    std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

		TaskType task = [p_task]() { (*p_task)(); };
		this->tasks_.push(task);

		return p_task->get_future();
	}
#elif __cplusplus >= 201402L
	template <typename F, typename... Args> auto submit(F&& f, Args&&... args) {
		std::function<decltype(f(args...))()> func =
		    std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		// 用异步操作封装
		auto p_task =
		    std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

		TaskType task = [p_task]() { (*p_task)(); };

		this->tasks_.push(task);
		return p_task->get_future();
	}
#endif

	~ThreadPool() {
		tasks_.stop();
		for (auto& worker : this->workers_) {
			if (worker.joinable()) {
				worker.join();
			}
		}
	}

	// 禁止拷贝
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	// 禁止移动
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

private:
	ThreadSafeQueue<TaskType> tasks_;  // 采用线程安全的队列
	std::vector<std::thread> workers_; // 线程集
};

} // namespace putils

#endif // THREADPOOL_THREADPOOL_HPP
