#include "data.h"
#include "helper.h"

#include <cstring>
#include <iostream>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

Query read_msg(int fd, size_t buf_size) {
  Query q;
  char buf[buf_size] = {};

  // Read length
  helper::read_all(fd, buf, 4);

  memcpy(&q.len, buf, 4);

  if (q.len > max_msg) {
    throw std::runtime_error("too long");
  }

  // Read data
  helper::read_all(fd, buf + 4, q.len);
  buf[q.len + 4] = '\0';

  q.msg = buf + 4;

  return q;
}

void send_msg(int fd, const char *msg, size_t buf_size) {
  uint32_t len = strlen(msg);

  char buf[buf_size];

  memcpy(buf, &len, 4);
  memcpy(buf + 4, msg, len);

  helper::write_all(fd, buf, len + 4);
}

void send_query(int connfd, const char *text) {
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

  int rv = connect(fd, (const sockaddr *)&addr, sizeof(addr));

  if (rv) {
    perror("connect()");
  }

  send_query(fd, "get a\n");
  send_query(fd, "del a\n");
  send_query(fd, "set a b\n");

  Query q = read_msg(fd, 4 + max_msg + 1);
  std::cout << q.msg << "\n";

  q = read_msg(fd, 4 + max_msg + 1);
  std::cout << q.msg << "\n";

  q = read_msg(fd, 4 + max_msg + 1);
  std::cout << q.msg << "\n";

  return 0;
}
