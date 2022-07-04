//
// Created by qinyu on 2022/7/3.
//

#include "ThreadPool.h"

struct ThreadPool {
  // threadPool维护一个任务队列
  TaskQueue *taskQueue;

  // threadPool相关的设定
  int shutdown; // 线程池关闭, 0:关闭, 1:打开
  int maxThreads;
  int minThreads;
  int workingNum; // 正在工作的线程
  int livingNum; // 存活的线程
  int killNums; // 要杀死的线程数量
  // 线程相关
  pthread_t *tid, mid;

  // 互斥锁
  pthread_mutex_t threadPoolMutex, workingIdMutex;
  pthread_cond_t  isFull, isEmpty;

};

/**
 * 工作的线程（任务队列任务的消费者）,N个
 * 1.线程池中维护了一定数量的工作线程,他们的作用是是不停的读任务队列,从里边取出任务并处理
 * 2.工作的线程相当于是任务队列的消费者角色。
 * 3.如果任务队列为空,工作的线程将会被阻塞 (使用条件变量 / 信号量阻塞)
 * 4.如果阻塞之后有了新的任务,由生产者将阻塞解除,工作线程开始工作
 * */
void *worker(void *arg) {
  // 这里是不是得加上一个条件变量?
  ThreadPool *threadPool = (ThreadPool*)arg;
  TaskQueue *taskQueue = threadPool->taskQueue;

  // 这里写成死循环, 也就是循环取任务.
  while (1) { // NOTE:我们这里是一个死循环, 那么怎么跳出这个循环呢? 注意我们这里有shutdown, 当执行关闭线程池的时候, 会killthreads
    // 只有一个线程通过, 取出任务, 并且执行
    pthread_mutex_lock(&threadPool->threadPoolMutex);
    while (getSize(taskQueue) == 0 && !threadPool->shutdown) { //QUESTION: 这里为什么使用while, 不使用if?
      pthread_cond_wait(&threadPool->isEmpty, &threadPool->threadPoolMutex); // 如果队列是空的, 就进行阻塞.

      if (threadPool->killNums > 0) {  // 如果要杀死线程数量大于0的话, 那就杀死线程.
        threadPool->killNums--;
        if (threadPool->livingNum > threadPool->minThreads) { // 线程最小的个数不能小于minThreads;
          threadPool->livingNum--;
          pthread_mutex_unlock(&threadPool->threadPoolMutex);
          killThreads(threadPool);
        }
      }
    }

    if (threadPool->shutdown) {
      pthread_mutex_unlock(&threadPool->threadPoolMutex);
      killThreads(threadPool);
    }

    // 取出一个任务. 当我们取出一个任务的时候, 任务队列就不满了 这个时候就可以就可以唤醒生产者.
    Task task = dequeue(taskQueue);
    pthread_cond_signal(&threadPool->isFull);
    pthread_mutex_unlock(&threadPool->threadPoolMutex);

    // 多个线程在执行
    printf("the thread id %ld is working\n", pthread_self());

    // 更新正在工作的线程数量(+1)
    pthread_mutex_lock(&threadPool->workingIdMutex);
    threadPool->workingNum++;
    int *num = (int*)getArgs(taskQueue);
    pthread_mutex_unlock(&threadPool->workingIdMutex);

    //执行任务函数
    printf("the thread %ld getting here\n", pthread_self());

//    printf("the args number is %d\n", *(int*)getArgs(taskQueue));

    callback(num); // NOTE: 这正的执行回调函数的地方.

    //任务执行完毕之后, 工作线程应该更新(-1)
    pthread_mutex_lock(&threadPool->workingIdMutex);
    threadPool->workingNum--;
    pthread_mutex_unlock(&threadPool->workingIdMutex);
  }
  return NULL;
}

/**
 * 管理者线程函数
 * 1.它的任务是周期性的对任务队列中的任务数量以及处于忙状态的工作线程个数进行检测
 * 2.当任务过多的时候,可以适当的创建一些新的工作线程
 * 3.当任务过少的时候,可以适当的销毁一些工作的线程
 * */
void *monitor(void *arg) {
  ThreadPool *threadPool = (ThreadPool*)arg;

  while (!threadPool->shutdown) {
    sleep(3); // 每3s监控一次
    if (threadPool->livingNum < getSize(threadPool->taskQueue) &&
        threadPool->livingNum < threadPool->maxThreads) {
      // 创建线程, 创建多少个线程呢?
      // 作者这里的做法是一次添加两个
      pthread_mutex_lock(&threadPool->threadPoolMutex);
      int counter = 0;
      for (int i = 0; i < threadPool->maxThreads &&
                      counter < 2 && threadPool->livingNum < threadPool->maxThreads; ++i) {
        if (threadPool->tid[i] == 0) {
          pthread_create(&threadPool->tid[i], NULL, worker, threadPool);
          counter++;
          threadPool->livingNum++; // 更新livingNum
        }
      }
      pthread_mutex_unlock(&threadPool->threadPoolMutex);
    }
    // 销毁线程 NOTE: 销毁线程还是不怎么理解.
    if (threadPool->workingNum * 2 < threadPool->livingNum &&
        threadPool->livingNum > threadPool->minThreads) {
      pthread_mutex_lock(&threadPool->threadPoolMutex);
      threadPool->killNums = 2;
      pthread_mutex_unlock(&threadPool->threadPoolMutex);
      // 让工作的线程自杀
      for (int i = 0; i < threadPool->killNums; ++i)
      {
        pthread_cond_signal(&threadPool->isFull);
      }
    }
  }

  return NULL;
}

ThreadPool *createThreadPool(int maxThreads, int minThreads) {
  // 在内部进行初始化
  // 首先我们对threadPool进行初始化:
  ThreadPool *threadPool = (ThreadPool *) malloc(sizeof(ThreadPool));
  threadPool->taskQueue = createTaskQueue(); // 先把这个capacity写死
  threadPool->tid = (pthread_t *) malloc(sizeof(pthread_t) * maxThreads);
  if (threadPool->tid == NULL) {
    printf("malloc thread id failed\n");
    return NULL;
  }
  memset(threadPool->tid, 0, sizeof(pthread_t) * maxThreads);

  threadPool->maxThreads = maxThreads;
  threadPool->minThreads = minThreads;
  threadPool->shutdown = 0;
  threadPool->workingNum = 0;
  threadPool->livingNum = minThreads; // 最少存活的个数和min threads相等
  threadPool->killNums = 0; // 要杀死的线程数.

  // NOTE: 这里既进行了初始化, 也增加了判断.
  if (pthread_mutex_init(&threadPool->threadPoolMutex, NULL) != 0 ||
      pthread_mutex_init(&threadPool->workingIdMutex, NULL) != 0 ||
      pthread_cond_init(&threadPool->isFull, NULL) != 0 ||
      pthread_cond_init(&threadPool->isEmpty, NULL) != 0) {

    printf("the mutex or condition init failed \n");
  }

  for (int i = 0; i < maxThreads; ++i) {
    /**
     * 一开始我在想为什么这里不是直接调用任务函数, 而是传递是以worker函数, 不仅仅自己的需求中分析的那样我们需要工作线程
     * 更是因为我们这里需要的是一个延迟的调用, 这里的worker更像是对真正线程函数的封装.
     * */
    pthread_create(&threadPool->tid[i], NULL, worker, threadPool); // 在这里我们直接启动了maxThreads的线程
  }
  pthread_create(&threadPool->mid, NULL, monitor, threadPool);
  return threadPool;
}


void destroyThreadPool(ThreadPool *threadPool) {
  if (threadPool == NULL) {
    return;
  }
  // 关闭线程池
  threadPool->shutdown = 1;

  // 阻塞回收管理者线程
  pthread_join(threadPool->mid, NULL);
  // 唤醒阻塞的消费者线程. NOTE: 这里为什么会唤醒消费者线程? 是不是确保存活的线程可以唤醒, 然后去自杀
  for (int i = 0; i < threadPool->livingNum; ++i) {
    pthread_cond_signal(&threadPool->isEmpty);
  }
  for (int i = 0; i < threadPool->maxThreads; i++) {
    pthread_join(threadPool->tid[i], NULL);
  }

  // 释放内存
  if (threadPool->taskQueue) destroyTaskQueue(threadPool->taskQueue);
  if (threadPool->tid) free(threadPool->tid);

  pthread_mutex_destroy(&threadPool->threadPoolMutex);
  pthread_mutex_destroy(&threadPool->workingIdMutex);

  pthread_cond_destroy(&threadPool->isFull);
  pthread_cond_destroy(&threadPool->isEmpty);
  free(threadPool);
  threadPool = NULL;
}

void threadPoolAdd(ThreadPool *threadPool, const Task *task) {
  // 主要是任务的添加, 给线程池添加任务的时候, 需要锁的保护
  pthread_mutex_lock(&threadPool->threadPoolMutex);

  //QUESTION: 为什么这里使用while循环, 而不是用if来判断
  //当主线程进入while这个代码块的时候, 说明任务队列已经满了, 然后主线程被阻塞
  //这里我们使用while而不使用if的原因是, 当主线程被唤醒的时候, 再次可以一直经过判断, 直到任务被取出的时候, 我们就可以跳出这个判断
  while (getSize(threadPool->taskQueue) == getCapacity(threadPool->taskQueue) &&
         !threadPool->shutdown) {
    // 阻塞生产者线程.
    pthread_cond_wait(&threadPool->isFull, &threadPool->threadPoolMutex);
  }

  if (threadPool->shutdown) {
    pthread_mutex_unlock(&threadPool->threadPoolMutex);
    return;
  }

  TaskQueue *taskQueue = threadPool->taskQueue;
  enqueue(taskQueue, task); // 任务入队

  // 当队列中有任务的时候, 我们将线程唤醒.
  pthread_cond_signal(&threadPool->isEmpty);
  pthread_mutex_unlock(&threadPool->threadPoolMutex);
}

void killThreads(ThreadPool *threadPool) {
  //NOTE: 一次杀死一个线程. 它的思路是: 当一个线程去调用这个函数的时候, 我们获取这个thread id, 然后循环遍历tid数组, 然后找到对应的id,对应等于0即可
  pthread_t tid = pthread_self();
  for (int i = 0; i < threadPool->maxThreads; ++i) {
    if (threadPool->tid[i] == tid) {
      threadPool->tid[i] = 0;
      printf("kill the thread, which id is %ld\n", tid);
      break;
    }
  }
  pthread_exit(NULL);
}
