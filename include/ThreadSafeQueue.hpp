/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2023/12/13 9:37
 * @version: 1.0
 * @description: 线程安全的队列
 ********************************************************************************/
#pragma once
#ifndef THREADPOOL_THREADSAFEQUEUE_HPP
#define THREADPOOL_THREADSAFEQUEUE_HPP

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

namespace putils {

template <typename T> class ThreadSafeQueue {
private:
	std::queue<T> data_;
	std::mutex mutex_{};
	std::condition_variable cond_{};
	bool stop_{false};

public:
	ThreadSafeQueue() = default;

	ThreadSafeQueue(const ThreadSafeQueue& others) {
		std::lock_guard<std::mutex> lk(others.mutex_); // 拿到其他的锁
		data_ = others.data_;
	}

	// 禁止拷贝赋值操作
	ThreadSafeQueue& operator=(const ThreadSafeQueue& others) = delete;

	/**
	 * 压队
	 * @param new_value 向队列中加入的新值
	 */
	void push(T new_value) {
		std::lock_guard<std::mutex> lk(mutex_);
		data_.push(std::move(new_value));
		cond_.notify_one();
	}
	/**
	 * 等待并弹出值
	 * 使用条件变量判断满足前置条件后，被唤醒的线程必须满足条件才能拿到锁
	 * @param value
	 * @return
	 */
	bool waitAndPop(T& value) {

		std::unique_lock<std::mutex> lk(mutex_);
		cond_.wait(lk, [this]() {
			return !this->data_.empty() || stop_;
		}); // 非空时才能读取内容
		// stop_条件下 data为空才能退出
		if (data_.empty())
			return false;

		value = std::move(data_.front());
		data_.pop();
		return true;
	}

	/**
	 * 等待并弹出值
	 * @return T类型的智能指针
	 */
	std::shared_ptr<T> waitAndPop() {
		std::unique_lock<std::mutex> lk(mutex_);
		cond_.wait(lk, [this]() {
			return !this->data_.empty() || stop_;
		}); // 非空时才能读取内容
		if (data_.empty())
			return nullptr;
		std::shared_ptr<T> ret(std::make_shared<T>(std::move(data_.front())));
		data_.pop();
		return ret;
	}

	bool empty() {
		std::lock_guard<std::mutex> lk(mutex_);
		return data_.empty();
	}

	/**
	 * 唤醒所有因数据为空的线程而被阻塞的线程并退出
	 */
	void stop() {
		{
			std::lock_guard<std::mutex> lk(mutex_);
			stop_ = true;
		}
		cond_.notify_all(); // 唤醒所有阻塞的队列
	}

	~ThreadSafeQueue() = default;
};

} // namespace putils

#endif // THREADPOOL_THREADSAFEQUEUE_HPP
