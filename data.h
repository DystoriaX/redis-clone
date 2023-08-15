#pragma once

#include <string>
#include <vector>

const size_t max_msg = 4096;

struct Query {
  uint32_t len;
  std::string msg;
};

struct Req {
  std::string payload;

  std::vector<uint8_t> serialize() const;
};

enum class status { ok, bad_request };

struct Res {
  status stat;
  std::string payload;

  std::string string_of_status() const;
  std::vector<uint8_t> serialize() const;
};
