#include "data.h"
#include "helper.h"

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

std::string Res::string_of_status() const {
  switch (this->stat) {
  case status::ok:
    return "OK";
  case status::bad_request:
    return "BAD_REQUEST";
  }
}

std::vector<uint8_t> Res::serialize() const {
  std::vector<uint8_t> ret;

  std::string response = this->string_of_status() + " " + payload;

  ret.resize(response.size() + 4);

  auto ret_data = ret.data();

  uint32_t len = response.size();
  memcpy(ret_data, &len, 4);
  memcpy(ret_data + 4, response.c_str(), len);

  return ret;
}
