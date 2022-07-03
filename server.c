#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> // 这个头文件包含了sys/socket.h, 所以我们套接字通信一般是包含这个头文件就可以了.

// 服务器端的代码
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
  // 4. 阻塞并且等待客户端连接
  struct sockaddr_in caddr;
  int addrlen = sizeof(caddr);
  int cfd = accept(fd, (struct sockaddr*)&caddr, (socklen_t*)&addrlen);

  //连接成功, 打印客户端的IP和端口
  char ip[32];

  printf("客户端IP %s, 端口: %d\n", inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(caddr.sin_port)); // 我们需要大端转换成小端


  //5 通信
  while (1) {
    char buff[1024]; // 用于接受客户端的数据
    int len = recv(cfd, buff, sizeof(buff), 0);
    if (len > 0) { // 如果大于0, 也就是接受了一定量的数据
      printf("客户端发送过来的数据: %s\n", buff);
      // 接受完客户端的数据之后, 我们这里为了图方便, 没有对收到的数据进行处理, 然后原封不动的把数据又发送给了客户端, 理论上来说这里需要对数据进行处理
      send(cfd, buff, len, 0);
    } else if (len == 0) {
      printf("客户端已经断开了连接\n");
      break;
    } else {
      perror("接受数据失败");
      break;
    }
  }

  close(fd);
  close(cfd);

  return 0;
}