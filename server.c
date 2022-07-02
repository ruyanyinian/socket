//
// Created by qinyu on 2022/6/29.
//
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
// 服务器端代码
typedef struct SockInfo {
  struct sockaddr_in addr;
  int fd;
} SockInfo;

SockInfo infos[512];
int main() {
  // 1. 创建监听的文件描述符
  int fd = socket(AF_INET, SOCK_STREAM, 0); // TCP通信方式
  if (fd == -1) {
    perror("创建监听的文件描述符失败");
  }
  // 2. 绑定服务器端的ip和端口，
  struct sockaddr_in sockaddrIn;
  sockaddrIn.sin_family = AF_INET;
  // 这里需要把指定的端口从主机字节序转换成网络字节序, 我们这里指定的本地端口, 我们需要找一个本地的没有被占用的端口
  sockaddrIn.sin_port = htons(9999);

  // 这个宏实际实际值为0, 对于0来说, 大端和小端是没有区别的, 因此是不需要进行转换的. 这里的0是指服务端去读网卡的实际id
  // 比如本地的ip是172.19.1.124, 然后我们指定INADDR_ANY, 也就是说我们传进去的实际的ip地址是172.19.1.124 并且也是直接转换成大端了, 如果我们想要绑定外网的ip地址, 那么我们就直接输入外网的ip就可以了
  sockaddrIn.sin_addr.s_addr = INADDR_ANY;

  int ret = bind(fd, (struct sockaddr*)&sockaddrIn, sizeof(sockaddrIn));
  if (ret == -1) {
    perror("绑定失败");
  }

  // 初始化结构数组
  int max = sizeof(infos) / sizeof(infos[0]); // 这个是数组的总大小
  for (int i = 0; i < max; ++i) {
    memset(&infos[i], 0, sizeof(infos[i]));
    infos[i].fd = -1; // Note: 这里fd = -1 也是有说法的
  }
  return 0;
}