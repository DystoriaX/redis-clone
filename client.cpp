#include "data.h"
#include "helper.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void send_query(int connfd, const char* text) {
  uint32_t len = strlen(text);

  if (len > max_msg) {
    throw std::runtime_error("too long");
  }

  char wbuf[4 + max_msg + 1] = {};

  memcpy(wbuf, &len, 4);
  memcpy(wbuf + 4, text, len);

  helper::write_all(connfd, wbuf, len + 4);
}

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd < 0) {
    perror("socket()");
  }

  sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);

  int rv = connect(fd, (const sockaddr*) &addr, sizeof(addr));

  if (rv) {
    perror("connect()");
  }

  send_query(fd, "oh no\n");
  send_query(fd, "oh noo\n");
  send_query(fd, "oh nooo\n");

  Query q = helper::read_msg(fd, 4 + max_msg + 1);
  std::cout << q.msg << "\n";

  q = helper::read_msg(fd, 4 + max_msg + 1);
  std::cout << q.msg << "\n";

  q = helper::read_msg(fd, 4 + max_msg + 1);
  std::cout << q.msg << "\n";

  return 0;
}
