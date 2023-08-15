#pragma once

#include <optional>
#include <unistd.h>
#include <vector>

namespace helper {
extern std::vector<std::string>
split(const std::string &, const char delimiter = ' ', const int count = -1);
void read_all(int fd, char *buf, size_t n);
void write_all(int fd, const char *buf, size_t n);
} // namespace helper
