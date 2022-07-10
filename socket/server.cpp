#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> // 这个头文件包含了sys/socket.h, 所以我们套接字通信一般是包含这个头文件就可以了.
#include "server.h"


// 线程池来进行服务器的并发

using namespace std;

typedef struct SockInfo {
  struct sockaddr_in addr;
  int fd;
} SockInfo;

struct PoolInfo {
  ThreadPool *p;
  int fd;
};
// Note: 这里我们需要一个数组进行存储结构体信息, 因为我们传入的是地址, 多个客户端的东西fd和addr是不一样的


//void acceptConn(void *) // 这个函数是任务函数, 是函数指针using callback = void(*)(void*);

void *working(void *arg) {
  SockInfo *pinfo = (struct SockInfo*)arg;

  // 连接成功打印端口ip信息
  char ip[32];
  printf("客户端IP %s, 端口: %d\n", inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(pinfo->addr.sin_port)); // 我们需要大端转换成小端

  //5 通信
  while (1) {
    char buff[1024]; // 用于接受客户端的数据
    int len = recv(pinfo->fd, buff, sizeof(buff), 0);
    if (len > 0) { // 如果大于0, 也就是接受了一定量的数据
      printf("客户端发送过来的数据: %s\n", buff);
      send(pinfo->fd, buff, len, 0);
    } else if (len == 0) {
      printf("客户端已经断开了连接\n");
      break;
    } else {
      perror("接受数据失败");
      break;
    }
  }

  close(pinfo->fd);
  pinfo->fd = -1;

  return NULL;
}

int main() {
  // 1. 创建监听的套接字
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("套接字创建失败");
    return -1;
  }

  // 2. 绑定本地的ip端口
  struct sockaddr_in sockaddrIn;
  // 2.1 sockaddrIn 这个变量的初始化
  sockaddrIn.sin_family = AF_INET;
  // 这里需要把指定的端口从主机字节序转换成网络字节序, 我们这里指定的本地端口, 我们需要找一个本地的没有被占用的端口
  sockaddrIn.sin_port = htons(9999);

  // 这个宏实际实际值为0, 对于0来说, 大端和小端是没有区别的, 因此是不需要进行转换的. 这里的0是指服务端去读网卡的实际id
  // 比如本地的ip是172.19.1.124, 然后我们指定INADDR_ANY, 也就是说我们传进去的实际的ip地址是172.19.1.124, 如果我们想要绑定外网的ip地址, 那么我们就直接输入外网的ip就可以了
  sockaddrIn.sin_addr.s_addr = INADDR_ANY;
  int ret = bind(fd, (struct sockaddr *)&sockaddrIn, sizeof(sockaddrIn));
  if (ret == -1) {
    perror("绑定失败");
  }

  // 3. 设置监听
  ret = listen(fd, 128);
  if (ret == -1) {
    perror("监听失败");
  }

  // 创建线程池, 分别指定最小的线程数和最大的线程数
  // 我们的任务函函数是accpetConn
//  ThreadPool threadPool(3, 8);
//
//  Task task(acceptConn, NULL);
//  threadPool.addTask(task); // 在这里已经开始启动线程了
//
/**
 *  这个是C接口
    ThreadPool *pool = threadpoolcreate(3, 8, 100) 初始化实例接口
    PoolInfo *info = (PoolInfo*) malloc(sizeof(PoolInfo)) 传递线程池和fd
    info->p = pool;
    info->fd = fd; // 这个是用于监听的文件描述符
    threadpooladd(pool, acceptConn, info); info会在 threadpool里面释放
    注意我们这里没有线程池的销毁的, 因为服务器肯定一直是保持接受的状态的, 需要在某一个时刻进行销毁
 *
 * */

  pthread_exit(NULL); // 主线程退出, 并不会影响所有的线程池中的线程运行
  return 0;
}


void acceptConn(void *arg) {
  // 从第4步开始, 下面的过程是等待和客户端建立连接.
  // 4. 阻塞并且等待客户端连接
  // poolinfo *poolinfo = (poolinfo*)arg;
  //
  int addrlen = sizeof(struct sockaddr_in);
  while (1) {
    struct SockInfo *pinfo = (SockInfo *) malloc(sizeof(SockInfo) * 1);
    pinfo->fd = accept(pinfo->fd, (struct sockaddr*)&pinfo->addr, (socklen_t*)&addrlen); // 这个是接受客户端的连接, 然后返回的通信的文件描述符
    if (pinfo->fd  == -1) {
      perror("建立连接失败");
      break;
    }
    threadPoolAdd(poolinfo->p, working, pinfo) 这里的参数是pinfo
  }

  close(poolinfo->fd); // 这里的监听的文件描述符可以关闭, 通信的文件描述符不能关闭, 因为它在子线程中使用的
  return 0;
}