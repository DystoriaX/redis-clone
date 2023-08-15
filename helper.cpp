#include "helper.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

// helper function
std::vector<std::string> helper::split(const std::string &s,
                                       const char delimiter, const int count) {
  size_t last = 0;
  int split_count = 0;

  std::vector<std::string> res;

  for (auto i = 0; i < s.size(); i++) {
    if (split_count == count) {
      break;
    }

    if (s[i] == delimiter) {
      if (i - last > 0) {
        res.push_back(s.substr(last, i - last));
        split_count += 1;
      }

      last = i + 1;
    }
  }

  if (s.size() - last > 0) {
    res.push_back(s.substr(last, s.size() - last));
  }

  return res;
}

/**
 * @brief Helper function to ensure that n bytes are read
 *
 * This is needed as read system call may not read all n bytes
 *
 * @param fd file descriptor
 * @param buf buffer to write the read characters
 * @param n number of bytes
 */
void helper::read_all(int fd, char *buf, size_t n) {
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
void helper::write_all(int fd, const char *buf, size_t n) {
  while (n > 0) {
    auto rv = write(fd, buf, n);

    if (rv <= 0) {
      throw std::runtime_error("write_all failed");
    }

    n -= rv;
    buf += rv;
  }
}
