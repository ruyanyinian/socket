//
// Created by qinyu on 2022/7/4.
//
#include "../threadpool/TaskQueueC_API.h"
#include <stdio.h>

void print(void *a) {
  int *pA = (int*)a;
  printf("the function will print %d\n", *pA);
}
int main() {
  void *pTaskQueue = createTaskQueue();
  int a = 10;
  void *task = createTask(print, &a);
  enqueue(pTaskQueue, task);
  TaskC *ptask = (TaskC*)dequeue(pTaskQueue);
  ptask->m_func(ptask->m_arg);
  return 0;
}
