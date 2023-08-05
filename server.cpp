#include "helper.h"
#include "parser.h"
#include "data.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/ip.h>

class EpollWrapper {
private:
  int epollfd;

public:
  EpollWrapper() {
    epollfd = epoll_create1(0);

    if (epollfd == -1) {
      perror("epoll_create1()");
      throw std::runtime_error("EpollWrapper: object instantiation failed");
    }
  }

  void add_fd(int fd, int flags) {
    epoll_event e;
    e.events = flags;
    e.data.fd = fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &e) == -1) {
      perror("epoll_ctl()");
      throw std::runtime_error("add_fd: epoll_ctl failed");
    }
  }

  void mod_fd(int fd, int flags) {
    epoll_event e;
    e.events = flags;
    e.data.fd = fd;

    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &e) == -1) {
      perror("epoll_ctl()");
      throw std::runtime_error("mod_fd: epoll_ctl failed");
    }
  }

  void del_fd(int fd) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
      perror("epoll_ctl()");
      throw std::runtime_error("del_fd: epoll_ctl failed");
    }
  }

  std::vector<epoll_event> wait_fd(int max_event, int timeout = -1) {
    std::vector<epoll_event> events(max_event);

    auto nfds = epoll_wait(epollfd, events.data(), max_event, timeout);

    if (nfds == -1) {
      perror("epoll_wait");
      throw std::runtime_error("wait_fd: epoll_wait failed");
    }

    events.resize(nfds);

    return events;
  }

  EpollWrapper(const EpollWrapper&) = delete;
  EpollWrapper& operator=(const EpollWrapper&) = delete;

  EpollWrapper(EpollWrapper&&) = delete;
  EpollWrapper& operator=(EpollWrapper&&) = delete;

  ~EpollWrapper() {
    close(epollfd);
  }
};

enum class connstate {
  req, res, end
};

struct Conn {
  int fd;
  sockaddr_in client_addr;
  socklen_t addr_len;
  connstate state;

  size_t rbuf_size = 0;
  std::array<uint8_t, 4 + max_msg> rbuf;

  size_t wbuf_size = 0;
  size_t wbuf_sent = 0;
  std::array<uint8_t, 4 + max_msg> wbuf;
};

void set_fd_nb(int);

void handle_conn(Conn*);

void handle_req(Conn*);
void try_fill_buffer(Conn*);
bool try_one_req(Conn*);

void handle_res(Conn*);
void try_flush_buffer(Conn*);

std::unique_ptr<Conn> accept_new_conn(int listenfd);
void run_server(int listenfd);

void set_fd_nb(int fd) {
  errno = 0;
  auto flags = fcntl(fd, F_GETFD);

  if (errno) {
    perror("fcntl() get");
    exit(1);
  }

  flags |= O_NONBLOCK;

  errno = 0;
  fcntl(fd, F_SETFD, flags);

  if (errno) {
    perror("fcntl() set");
    exit(1);
  }
}

std::unique_ptr<Conn> accept_new_conn(int listenfd) {
  std::unique_ptr<Conn> new_conn(new Conn());

  int fd = accept(listenfd, (sockaddr*) &new_conn->client_addr,
                  &new_conn->addr_len);

  if (fd < 0) {
    perror("accept() error");
    throw std::runtime_error("Cannot accept new connection");
  }

  new_conn->fd = fd;
  new_conn->state = connstate::req;
  return new_conn;
}

void handle_req(Conn* conn) {
  try_fill_buffer(conn);

  conn->wbuf_sent = 0;
  conn->wbuf_size = 0;
  while (try_one_req(conn)) {
    conn->state = connstate::res;
  }
}

void try_fill_buffer(Conn* conn) {
  ssize_t rv = 0;

  do {
    auto cap = sizeof(conn->rbuf) - conn->rbuf_size;

    errno = 0;
    rv = read(conn->fd, conn->rbuf.data() + conn->rbuf_size, cap);

    // keep trying while it's interrupted
  } while (rv < 0 && errno == EINTR);

  // EAGAIN means try again
  if (rv < 0 && errno == EAGAIN) {
    return;
  }

  // Error
  if (rv < 0) {
    std::cerr << "Read error\n";
    conn->state = connstate::end;
    return;
  }

  // EOF
  if (rv == 0) {
    conn->state = connstate::end;
    return;
  }

  conn->rbuf_size += rv;
}

bool try_one_req(Conn* conn) {
  std::optional<Req> o_req;

  try {
    o_req = parser::parse_req(conn->rbuf.data(), conn->rbuf_size);
  } catch (const RequestTooLong& e) {
    conn->state = connstate::end;
    return false;
  }

  if (!o_req) {
    return false;
  }

  Req req = o_req.value();

  std::cout << "client says: " << req.payload << "\n";

  // append to what needs to be written out

  auto serialized_res = req.serialize();

  memcpy(&conn->wbuf[conn->wbuf_size], serialized_res.data(), serialized_res.size());
  conn->wbuf_size += serialized_res.size();

  // reclaim read buffer. Note: can be further optimized
  uint32_t len = req.payload.size();

  size_t remain = conn->rbuf_size - (4 + len);
  if (remain) {
    memmove(conn->rbuf.data(), &conn->rbuf[4 + len], remain);
  }

  conn->rbuf_size = remain;

  return true;
}

void handle_res(Conn* conn) {
  ssize_t rv = 0;

  do {
    auto remain = conn->wbuf_size - conn->wbuf_sent;
    rv = write(conn->fd, conn->wbuf.data() + conn->wbuf_sent, remain);
  } while (rv < 0 && errno == EINTR);

  if (rv < 0 && errno == EAGAIN) {
    return;
  }

  if (rv < 0) {
    std::cerr << "write error\n";
    conn->state = connstate::end;
    return;
  }

  conn->wbuf_sent += rv;

  if (conn->wbuf_sent == conn->wbuf_size) {
    conn->wbuf_sent = 0;
    conn->wbuf_size = 0;
    conn->state = connstate::req;
  }
}

void handle_conn(Conn* conn) {
  if (conn->state == connstate::req) {
    handle_req(conn);
  } else if (conn->state == connstate::res) {
    handle_res(conn);
  }
}

void run_server(int listenfd) {
  EpollWrapper epoll;
  epoll.add_fd(listenfd, EPOLLIN);

  const int max_events = 10;

  std::unordered_map<int, std::unique_ptr<Conn>> connections;

  while (true) {
    auto events = epoll.wait_fd(max_events);
    // std::cerr << "waited\n";

    for (auto& ev : events) {
      if (ev.data.fd == listenfd) {
        auto conn = accept_new_conn(listenfd);

        set_fd_nb(conn->fd);
        epoll.add_fd(conn->fd, EPOLLIN | EPOLLERR);
        // std::cerr << "Client with fd " << conn->fd << " has been accepted\n";

        connections[conn->fd] = std::move(conn);
      } else {
        Conn* conn = connections.at(ev.data.fd).get();
        // std::cerr << "Handling client with fd " << conn->fd << "\n";
        handle_conn(conn);
        // std::cerr << "Client has state " << static_cast<int>(conn->state) << "\n";

        if (conn->state == connstate::res) {
          epoll.mod_fd(conn->fd, EPOLLOUT | EPOLLERR);
        } else if (conn->state == connstate::end) {
          epoll.del_fd(conn->fd);
          std::cerr << "client disconnected\n";
        }
      }
    }
  }

  // while (true) {
  //   sockaddr_in client_addr = {};
  //   socklen_t socklen = sizeof(client_addr);

  //   std::cout << "listening...\n";
  //   int connfd = accept(listenfd, (sockaddr*) &client_addr, &socklen);
  //   if (connfd < 0) {
  //     continue;
  //   }

  //   std::cout << "new client accepted\n";

  //   while (true) {
  //     try {
  //       handle_conn(connfd);
  //     } catch (const std::runtime_error& e) {
  //       std::cout << e.what() << "\n";
  //       break;
  //     }
  //   }
  // close(connfd);
  // std::cout << "server has closed the connection...\n";

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

  run_server(fd);

  return 0;
}
