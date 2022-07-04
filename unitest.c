//
// Created by qinyu on 2022/7/4.
//
#include "threadpool/TaskQueueC_API.h"
#include <stdio.h>

int main() {
  TaskQueue *taskQueue = createTaskQueue();
  int ans = getSize(taskQueue);
  printf("the %d", ans);
  return 0;
}