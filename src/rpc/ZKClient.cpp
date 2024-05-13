/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/22 15:53:36
 * @version: 1.0
 * @description:
 ********************************************************************************/
#include "rpc/ZKClient.h"
#include "base/Logger.h"
#include <cstdlib>
#include <semaphore.h>
#include <string>
#include <vector>
#include <zookeeper/zookeeper.h>
#include <zookeeper/zookeeper.jute.h>

static void global_watcher(zhandle_t* zh, int type, int state, const char* path,
                           void* watchrCtx) {
	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
			sem_t* sem = (sem_t*)zoo_get_context(zh);
			sem_post(sem);
		}
	}
}

ZKClient::~ZKClient() {
	if (zhandle_ != nullptr) {
		// 关闭连接 并 释放资源
		zookeeper_close(zhandle_);
	}
}

void ZKClient::start() {
	zhandle_ = zookeeper_init("127.0.0.1:2181", global_watcher, 30000, nullptr,
	                          nullptr, 0);
	if (zhandle_ == nullptr) {
		ERROR_LOG << "zookeeper_init error!";
		exit(EXIT_FAILURE);
	}

	sem_t sem;
	sem_init(&sem, 0, 0);
	zoo_set_context(zhandle_, &sem);

	// 等待成功注册
	sem_wait(&sem);
	INFO_LOG << "zookeeper_init success!";
}

void ZKClient::create(const char* path, const char* data, int data_len,
                      int state) {
	constexpr int bufferlen = 128;
	char path_buffer[bufferlen];
	bool flag = check_znode_exists(path);
	// 对应的节点不存在
	if (!flag) {
		flag = zoo_create(zhandle_, path, data, data_len, &ZOO_OPEN_ACL_UNSAFE,
		                  state, path_buffer, bufferlen);
		if (flag == ZOK) {
			INFO_LOG << "znode create success... path: " << path;
		} else {
			ERROR_LOG << "flag: " << flag;
			ERROR_LOG << "znode create error... path: " << path;
			exit(EXIT_FAILURE);
		}
	}else {
        INFO_LOG << "znode create filure...exist: " << path;
    }
}
std::string ZKClient::get_data(const char* path) {
	char data_buffer[128];
	int bufferlen = sizeof(data_buffer);
	int flag = zoo_get(zhandle_, path, 0, data_buffer, &bufferlen, nullptr);
	if (flag != ZOK) {
		ERROR_LOG << "get znode error... path: " << path;
		return std::string{};
	} else {
		return std::string(data_buffer);
	}
}

bool ZKClient::check_znode_exists(const char *path){
	auto flag = zoo_exists(zhandle_, path, 0, nullptr);
	return !(flag == ZNONODE);
}

std::vector<std::string> ZKClient::get_children(const char *path){
	if(!check_znode_exists(path)) return {}; // 节点不存在 则不必继续下去了
	std::vector<std::string> ret;
	struct String_vector children;
	zoo_get_children(zhandle_, path, 0,&children);
	for(int i = 0;i < children.count;i++){
		ret.emplace_back(children.data[i]);
	}
	return ret;
}
