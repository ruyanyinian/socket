#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> // 这个头文件包含了sys/socket.h, 所以我们套接字通信一般是包含这个头文件就可以了.
#include "server.h"
// 线程池来进行服务器的并发


typedef struct SockInfo {
  struct sockaddr_in addr;
  int fd;
} SockInfo;


// Note: 这里我们需要一个数组进行存储结构体信息, 因为我们传入的是地址, 多个客户端的东西fd和addr是不一样的
// 如果不定义数组的话, 我们第一个客户端正在通信的同时就可能
SockInfo infos[512];



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

  int ret = bind(fd, (struct sockaddr*)&sockaddrIn, sizeof(sockaddrIn));
  if (ret == -1) {
    perror("绑定失败");
  }

  // 3. 设置监听
  ret = listen(fd, 128);
  if (ret == -1) {
    perror("监听失败");
  }

  // 初始化结构数组
  int max = sizeof(infos) / sizeof(infos[0]); // 这个是数组的总大小
  for (int i = 0; i < max; ++i) {
    bzero(&infos[i], sizeof(infos[i])); // bzero和memset都是对内存进行初始化为0, 只不过bzero这里比memset少些一个0
    infos[i].fd = -1; // Note: 这里fd = -1 用于下面的判断

  }
  // 4. 阻塞并且等待客户端连接
  int addrlen = sizeof(struct sockaddr_in);
  while (1) {
    struct SockInfo *pinfo = NULL;
    for (int i = 0; i < max; ++i) {
      if (infos[i].fd == -1) {
        pinfo = &infos[i];
        break;
      }
    }

    int cfd = accept(fd, (struct sockaddr*)&infos->addr, (socklen_t*)&addrlen);
    pinfo->fd = cfd;
    if (cfd == -1) {
      perror("建立连接失败");
      continue;
    }
    // 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, pinfo);
    pthread_detach(tid); // 注意这里不能使用pthread_join(), 这是因为这是个阻塞函数, 也就是说我们必须等到子线程执行完毕。这样的话我们的效率就低了。
  }

  close(fd); // 这里的监听的文件描述符可以关闭, 通信的文件描述符不能关闭, 因为它在子线程中使用的
  return 0;
}