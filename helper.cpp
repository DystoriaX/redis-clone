#include "helper.h"

#include <stdexcept>
#include <cstring>
#include <iostream>

/**
 * @brief Helper function to ensure that n bytes are read
 *
 * This is needed as read system call may not read all n bytes
 *
 * @param fd file descriptor
 * @param buf buffer to write the read characters
 * @param n number of bytes
 */
void helper::read_all(int fd, char* buf, size_t n) {
  while (n > 0) {
    auto rv = read(fd, buf, n);

    if (rv <= 0) {
      throw std::runtime_error("read_all failed");
    }

    n -= rv;
    buf += rv;
  }
}

/**
 * @brief Helper function to ensure that n bytes are written
 *
 * This is needed as write system call may not write all n bytes
 *
 * @param fd file descriptor
 * @param buf buffer to be written to the fd
 * @param n number of bytes
 */
void helper::write_all(int fd, const char* buf, size_t n) {
  while (n > 0) {
    auto rv = write(fd, buf, n);

    if (rv <= 0) {
      throw std::runtime_error("write_all failed");
    }

    n -= rv;
    buf += rv;
  }
}

/**
 * @brief reads a message from fd stream
 *
 * @param fd
 * @param buf_size
 * @return Query
 */
Query helper::read_msg(int fd, size_t buf_size) {
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

void helper::send_msg(int fd, const char* msg, size_t buf_size) {
  uint32_t len = strlen(msg);

  char buf[buf_size];

  memcpy(buf, &len, 4);
  memcpy(buf + 4, msg, len);

  helper::write_all(fd, buf, len + 4);
}
