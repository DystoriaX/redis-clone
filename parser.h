#pragma once

#include "data.h"

#include <optional>

class MessageTooLong : public std::invalid_argument {
public:
  MessageTooLong(const std::string &what_arg);
  MessageTooLong(const char *what_arg);
};

namespace parser {
std::optional<Req> parse_req(const uint8_t *data, size_t size);
std::optional<std::string> parse_res(const uint8_t *data, size_t size);
} // namespace parser
