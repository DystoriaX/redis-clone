#pragma once

#include <unistd.h>

namespace helper {
  void read_all(int fd, char* buf, size_t n);
  void write_all(int fd, const char* buf, size_t n);
}
