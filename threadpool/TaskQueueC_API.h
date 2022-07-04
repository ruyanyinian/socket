//
// Created by qinyu on 2022/7/3.
//

/**
 * TaskQueue *createTaskQueue()
 * void destroyTaskQueue(TaskQueue *taskQueue)
 * int getSize(TaskQueue *taskQueue)
 * int getCapacity(TaskQueue *taskQueue)
 * void enqueue(void *(*taskFunc)(void *),  void *args)
 *
 * */
#ifndef SOCKET_TASKQUEUEC_API_H
#define SOCKET_TASKQUEUEC_API_H
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
  typedef struct Task Task;
  typedef struct TaskQueue TaskQueue;
  // 创建taskqueue的对象
  TaskQueue *createTaskQueue();
  // 释放对象
  void destroyTaskQueue(TaskQueue* taskQueue);
  int getSize(TaskQueue* taskQueue);
  int getCapacity(TaskQueue *taskQueue);
  // 入队
  void enqueue(TaskQueue *taskQueue, const Task *task);
  Task *dequeue(TaskQueue *taskQueue);
#ifdef __cplusplus
}
#endif

#endif //SOCKET_TASKQUEUEC_API_H
