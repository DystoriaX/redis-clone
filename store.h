#pragma once

#include <unordered_map>

class Store {
private:
  std::unordered_map<std::string, std::string> umap;

public:
  Store();
  Store(const Store &) = delete;
  Store operator=(const Store &) = delete;

  std::optional<std::string> get(const std::string &);
  bool set(const std::string &, const std::string &);
  bool del(const std::string &);
};
