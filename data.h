#pragma once

#include <string>
#include <vector>

const size_t max_msg = 4096;

struct Query {
  uint32_t len;
  std::string msg;
};

enum class cmd {
  get, set, del
};

// possible to use variant???
struct Req {
  cmd type;
  std::string payload;
  std::vector<uint8_t> serialize() const;
};

enum class status {
  ok, bad_request
};

struct Res {
  status stat;
  std::string payload;
  std::string serialize() const;
};
