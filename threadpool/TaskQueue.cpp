//
// Created by qinyu on 2022/7/3.
//

#include "TaskQueue.h"
Task::Task() {
  m_func = nullptr;
  m_arg = nullptr;
}

Task::Task(Callback func, void *arg) {
  m_func = func;
  m_arg = arg;
}

Task::~Task() {

}

TaskQueue::TaskQueue() {
  pthread_mutex_init(&taskQueueMutex, nullptr);
}


void TaskQueue::enTaskQueue(const Task &task) {
  pthread_mutex_lock(&taskQueueMutex);
  taskQueue.push(task);
  pthread_mutex_unlock(&taskQueueMutex);
}

void TaskQueue::enTaskQueue(Callback func, void *arg) {
  pthread_mutex_lock(&taskQueueMutex);
  Task task;
  task.m_func = func;
  task.m_arg = arg;
  taskQueue.push(task);
  pthread_mutex_unlock(&taskQueueMutex);
}

Task TaskQueue::deTaskQueue() {
  Task t = taskQueue.front();
  return t;
}

int TaskQueue::getSize() {
  return taskQueue.size();
}

int TaskQueue::getCapacity() {
  return CAPACITY;
}
