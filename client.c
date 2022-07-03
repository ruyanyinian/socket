//
// Created by qinyu on 2022/7/3.
//
//
// Created by qinyu on 2022/6/5.
//
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> // 这个头文件包含了sys/socket.h, 所以我们套接字通信一般是包含这个头文件就可以了.

// 客户端的代码
int main() {
  // 1. 创建用于通信的套接字
  int fd = socket(AF_INET, SOCK_STREAM, 0); // ipv4协议
  if (fd == -1) {
    perror("套接字创建失败");
    return -1;
  }
  // 2. 绑定服务器的ip端口
  struct sockaddr_in sockaddrIn;

  // 2.1 sockaddrIn 这个变量的初始化
  sockaddrIn.sin_family = AF_INET;
  // 这里需要把指定的端口从主机字节序转换成网络字节序, 我们这里指定的本地端口, 我们需要找一个本地的没有被占用的端口
  sockaddrIn.sin_port = htons(9999); // 服务器绑定的端口

  // 这个宏实际实际值为0, 对于0来说, 大端和小端是没有区别的, 因此是不需要进行转换的. 这里的0是指服务端去读网卡的实际id

  inet_pton(AF_INET, "172.19.1.124", &sockaddrIn.sin_addr.s_addr);
  sockaddrIn.sin_addr.s_addr = INADDR_ANY;

  int ret = connect(fd, (struct sockaddr*)&sockaddrIn, sizeof(sockaddrIn));
  if (ret == -1) {
    perror("连接失败");
    return -1;
  }

  int number = 0;
  //3 通信
  while (1) {
    char buff[1024]; // 发送数据
    sprintf(buff, "hello world, %d...\n", number++);
    send(fd, buff, strlen(buff)+1, 0);

    // 接受服务器端响应的数据
    memset(buff, 0, sizeof(buff));
    int len = recv(fd, buff, sizeof(buff), 0);
    if (len > 0) { // 如果大于0, 也就是接受了一定量的数据
      printf("服务器发送过来的数据: %s\n", buff);
    } else if (len == 0) {
      printf("服务器已经断开了连接\n");
      break;
    } else {
      perror("接受数据失败");
      break;
    }
    sleep(1);
  }
  close(fd);

  return 0;
}