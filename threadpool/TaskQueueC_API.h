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
  // NOTE: 我们是不是包一层C接口的时候, 只需要用到基础类型就可以了, 其他的类型是不需要的? 比如Task *createTask(); 可以写成 void *createTask() \
  这里全部变成基础类型的就可以了, 然后返回值或者形参本来是具体的类的指针的, 这下全部变成void*, 然后通过强制类型转换就可以了.
  typedef void (*Callback)(void*);
  typedef struct TaskC {
    Callback m_func;
    void *m_arg;
  }TaskC;

  void *createTaskQueue();
  void *createTask(Callback func, void *arg);
  // 释放对象
  void destroyTaskQueue(void* taskQ);
  void destroyTask(void *task);
  int getSize(void *taskQueue);
  int getCapacity(void *taskQueue);
  // 入队
  void enqueue(void *taskQueue, void *task);
  void *dequeue(void *taskQueue);


#ifdef __cplusplus
}
#endif

#endif //SOCKET_TASKQUEUEC_API_H
