#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void echo(int connfd) {
  char rbuf[64] = {};

  ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);

  if (n < 0) {
    std::cerr << "read() error\n";
    return;
  }

  std::cout << "client says: " << rbuf << "\n";

  write(connfd, rbuf, strlen(rbuf));
}


int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  int val = 1;

  // Note: what do SO_REUSEADDR and SOL_SOCKET do?
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(0);

  if (bind(fd, (const sockaddr*) &addr, sizeof(addr))) {
    perror("bind()");
  }

  if (listen(fd, SOMAXCONN)) {
    perror("listen()");
  }

  while (true) {
    sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);

    std::cout << "listening...\n";
    int connfd = accept(fd, (struct sockaddr*) &client_addr, &socklen);
    std::cout << "new client accepted\n";

    if (connfd < 0) {
      continue;
    }

    echo(connfd);
    close(connfd);
  }

  return 0;
}
