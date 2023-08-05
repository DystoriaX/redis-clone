#pragma once

#include "data.h"

#include <optional>

class RequestTooLong : public std::invalid_argument {
public:
  RequestTooLong(const std::string& what_arg);
  RequestTooLong(const char* what_arg);
};

namespace parser {
  std::optional<Req> parse_req(const uint8_t* data, size_t size);
  std::optional<Res> parse_res(const uint8_t* data, size_t size);
}
