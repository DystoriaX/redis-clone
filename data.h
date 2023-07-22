#pragma once

#include <string>

const size_t max_msg = 4096;

struct Query {
  uint32_t len;
  std::string msg;
};
