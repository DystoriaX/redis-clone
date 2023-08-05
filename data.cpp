#include "data.h"

#include <cstring>

std::vector<uint8_t> Req::serialize() const {
  std::vector<uint8_t> ret;

  ret.resize(4 + this->payload.size());

  // should be safe...
  auto ret_data = ret.data();

  uint32_t len = this->payload.size();
  memcpy(ret_data, &len, 4);
  memcpy(ret_data + 4, this->payload.c_str(), len);

  return ret;
}
