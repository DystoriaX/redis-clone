#include "helper.h"

#include <stdexcept>

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
    auto rv = write(fd, buf, n);

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
