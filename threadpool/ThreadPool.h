//
// Created by qinyu on 2022/7/3.
//

#ifndef SOCKET_PRACTICE_THREADPOOL_H
#define SOCKET_PRACTICE_THREADPOOL_H
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "TaskQueueC_API.h"

typedef struct ThreadPool ThreadPool;

ThreadPool *createThreadPool(int maxThreads, int minThreads); // threadPool的初始化
void destroyThreadPool(ThreadPool *threadPool);
void threadPoolAdd(ThreadPool *threadPool, const Task *task);
void killThreads(ThreadPool *threadPool);

#endif //SOCKET_PRACTICE_THREADPOOL_H
