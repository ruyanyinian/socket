//
// Created by qinyu on 2022/7/3.
//

#ifndef SOCKET_PRACTICE_TASKQUEUE_H
#define SOCKET_PRACTICE_TASKQUEUE_H
#include <pthread.h>
#ifdef  __cplusplus
#include <queue>


#include <stdio.h>
#include <stdlib.h>

#define CAPACITY 100
#define ERROR -99 // 一般来说我们定义一个
#define CORRECT 0

typedef void (*Callback)(void*);
using namespace std;

struct Task {
public:
  Task();
  Task(Callback func, void *arg);
  ~Task();

public:
  Callback m_func;
  void *m_arg;
};

struct TaskQueue {
public:
  TaskQueue();
  void enTaskQueue(const Task &task);
  void enTaskQueue(Callback func, void *arg);
  Task deTaskQueue();
  int getSize();
  int getCapacity();
  ~TaskQueue();
private:
  std::queue<Task> taskQueue;
  pthread_mutex_t taskQueueMutex; // 用来保护任务队列的锁
};
#endif


#endif //SOCKET_PRACTICE_TASKQUEUE_H
