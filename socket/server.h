//
// Created by qinyu on 2022/7/7.
//

#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H
#include <pthread.h>
#include "../thirdParty/include/taskqueue.h"
#include "../thirdParty/threadpool/threadpool.h"
void acceptConn(void *arg); // 这个是任务函数
#endif //SOCKET_SERVER_H
