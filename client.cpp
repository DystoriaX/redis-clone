#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>

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

  std::string msg;

  std::cin >> msg;

  write(fd, msg.c_str(), msg.size());

  char rbuf[64] = {};
  ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);

  if (n < 0) {
    perror("read()");
  }

  std::cout << "server says: " << msg << "\n";

  return 0;
}
