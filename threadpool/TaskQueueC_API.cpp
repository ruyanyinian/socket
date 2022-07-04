//
// Created by qinyu on 2022/7/3.
//
#include "TaskQueue.h"
#include <string.h>
#include "TaskQueueC_API.h"

// 创建taskqueue的对象
TaskQueue *createTaskQueue() {
  TaskQueue *taskQueue = new TaskQueue();
  return taskQueue;
}

// 释放对象
void destroyTaskQueue(TaskQueue *taskQueue) {

}
int getSize(TaskQueue *taskQueue) {
  return taskQueue->getSize();
}

int getCapacity(TaskQueue *taskQueue) {
  return taskQueue->getCapacity();
}
// 入队

void enqueue(TaskQueue *taskQueue, const Task *task) {
  taskQueue->enTaskQueue(*task);
}

Task *dequeue(TaskQueue *taskQueue) {
  Task task = taskQueue->deTaskQueue();
  Task *ptask = (Task*) malloc(sizeof(task) * 1);
  memcpy(ptask, &task, sizeof(Task)*1);
  return ptask;
}

