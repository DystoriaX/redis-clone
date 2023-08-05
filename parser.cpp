#include "parser.h"

#include <cstring>

RequestTooLong::RequestTooLong(const std::string& what_arg) : std::invalid_argument(what_arg) {}
RequestTooLong::RequestTooLong(const char* what_arg) : std::invalid_argument(what_arg) {}

std::optional<Req> parser::parse_req(const uint8_t* data, size_t size) {
  if (size < 4) {
    return std::nullopt;
  }

  uint32_t len = 0;
  memcpy(&len, data, 4);

  if (len > max_msg) {
    throw new RequestTooLong("message length too long");
  }

  // not enough data
  if (len + 4 > size) {
    return std::nullopt;
  }

  Req req{cmd::get, std::string((const char*) data + 4, len)};

  return req;
}

std::optional<Res> parser::parse_res(const uint8_t* data, size_t size) {
  return std::nullopt;
}
