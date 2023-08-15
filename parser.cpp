#include "parser.h"
#include "helper.h"

#include <cstring>

MessageTooLong::MessageTooLong(const std::string &what_arg)
    : std::invalid_argument(what_arg) {}
MessageTooLong::MessageTooLong(const char *what_arg)
    : std::invalid_argument(what_arg) {}

std::optional<Req> parser::parse_req(const uint8_t *data, size_t size) {
  if (size < 4) {
    return std::nullopt;
  }

  uint32_t len = 0;
  memcpy(&len, data, 4);

  if (len > max_msg) {
    throw new MessageTooLong("message length too long");
  }

  // not enough data
  if (len + 4 > size) {
    return std::nullopt;
  }

  Req req{std::string((const char *)data + 4, len)};

  return req;
}

std::optional<std::string> parser::parse_res(const uint8_t *data, size_t size) {
  if (size < 4) {
    return std::nullopt;
  }

  uint32_t len = 0;
  memcpy(&len, data, 4);

  if (len > max_msg) {
    throw new MessageTooLong("message length too long");
  }

  if (len + 4 > size) {
    return std::nullopt;
  }

  std::string payload{(const char *)data + 4, len};

  return payload;

  // auto sp = helper::split(payload, ' ', 1);

  // if (sp.size() != 2) {
  //   throw -1;
  // }

  // status stat;

  // auto stat_str = sp[0];
  // auto payload = sp[1];

  // if (stat_str == "OK") {
  //   stat = status::ok;
  // } else

  // Res res{std::string()};

  // return std::nullopt;
}
