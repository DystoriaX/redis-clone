#include "store.h"

Store::Store() : umap() {}

std::optional<std::string> Store::get(const std::string &key) {
  if (this->umap.find(key) == this->umap.end()) {
    return std::nullopt;
  }

  return this->umap.at(key);
}

bool Store::set(const std::string &key, const std::string &value) {
  this->umap[key] = value;
  return true;
}

bool Store::del(const std::string &key) {
  auto it = this->umap.find(key);

  if (it == this->umap.end()) {
    return false;
  }

  this->umap.erase(it);

  return true;
}
