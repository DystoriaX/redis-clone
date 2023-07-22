#include "helper.h"
#include "data.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void handle_conn(int connfd) {
  Query q = helper::read_msg(connfd, 4 + max_msg + 1);

  std::cout << "server received: " << q.msg << "\n";

  // echo reply
  helper::send_msg(connfd, q.msg.c_str(), 4 + max_msg + 1);
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
    int connfd = accept(fd, (sockaddr*) &client_addr, &socklen);
    if (connfd < 0) {
      continue;
    }

    std::cout << "new client accepted\n";

    while (true) {
      try {
        handle_conn(connfd);
      } catch (const std::runtime_error& e) {
        std::cout << e.what() << "\n";
        break;
      }
    }

    close(connfd);
    std::cout << "server has closed the connection...\n";
  }

  return 0;
}
