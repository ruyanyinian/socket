//
// Created by qinyu on 2022/7/3.
//
#include "TaskQueue.h"
#include <string.h>
#include "TaskQueueC_API.h"
// 创建taskqueue的对象
void *createTaskQueue() {
  TaskQueue *taskQueue = new TaskQueue();
  return taskQueue;
}

void *createTask(Callback func, void *arg) {
  Task *task = new Task(func, arg);
  return task;
}

// 释放对象
void destroyTaskQueue(void *taskQ) {
  TaskQueue *taskQueue = (TaskQueue *)taskQ;
  if (taskQ != nullptr) {
    //delete taskQueue;  FIXME: taskqueue.cpp确实没有定义析构函数
    taskQ = nullptr;
  }
}

void destroyTask(void *task) {
  Task *pTask = (Task*)task;
  if (pTask != nullptr) {
    delete pTask;
    pTask = nullptr;
  }
}

int getSize(void *taskQ) {
  TaskQueue *taskQueue = (TaskQueue*)taskQ;
  return taskQueue->getSize();
}

int getCapacity(void *taskQ) {
  TaskQueue *taskQueue = (TaskQueue *)taskQ;
  return taskQueue->getCapacity();
}


// 入队
void enqueue(void *taskQ, void *ptask) {
  TaskQueue *taskQueue = (TaskQueue *)taskQ;
  Task *task = (Task *)ptask;
  taskQueue->enTaskQueue(*task);
}

void *dequeue(void *taskQ) {
  TaskQueue *taskQueue = (TaskQueue*)taskQ;
  Task t = taskQueue->deTaskQueue();
  Task *task = new Task(t.m_func, t.m_arg); // NOTE: 注意对task这里需要进行析构
  return task;
}

