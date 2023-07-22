#pragma once

#include "data.h"

#include <unistd.h>

namespace helper {
  void read_all(int fd, char* buf, size_t n);
  void write_all(int fd, const char* buf, size_t n);
  Query read_msg(int fd, size_t buf_size);
  void send_msg(int fd, const char* msg, size_t buf_size);
}
