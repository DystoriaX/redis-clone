#include "cmd.h"
#include "helper.h"

#include <vector>

using namespace cmd;
using helper::split;

UnknownCommand::UnknownCommand(const std::string &what_arg)
    : std::invalid_argument(what_arg) {}

UnknownCommand::UnknownCommand(const char *what_arg)
    : std::invalid_argument(what_arg) {}

std::unique_ptr<Command> Command::deserialize(std::string s) {
  std::vector<std::string> sp = split(s);

  std::string &cmd = sp[0];

  if (cmd == "get") {
    return Get::deserialize(s);
  } else if (cmd == "set") {
    return Set::deserialize(s);
  } else if (cmd == "del") {
    return Del::deserialize(s);
  } else {
    throw UnknownCommand("unknown command " + cmd);
  }
}

Get::Get(const std::string &_key) : key(_key) {}

const std::string &Get::get_key() const { return this->key; }

std::unique_ptr<Get> Get::deserialize(std::string s) {
  std::vector<std::string> sp = split(s);

  if (sp.size() != 2 || sp[0] != "get") {
    throw UnknownCommand("invalid format for get command. Format: get k");
  }

  auto key = sp[1];
  return std::make_unique<Get>(key);
}

std::string Get::serialize() const { return "get " + this->key; }

Result Get::execute(Store &store) const {
  auto o_value = store.get(this->key);

  if (!o_value) {
    return Result{Status::failure_ne,
                  "the key " + this->key + " does not exist"};
  }

  return Result{Status::success, o_value.value()};
}

Set::Set(const std::string &_key, const std::string &_value)
    : key(_key), value(_value) {}

const std::string &Set::get_key() const { return this->key; }

const std::string &Set::get_value() const { return this->value; }

std::unique_ptr<Set> Set::deserialize(std::string s) {
  std::vector<std::string> sp = split(s);

  if (sp.size() != 3 || sp[0] != "set") {
    throw UnknownCommand("invalid format for set command. Format: set k v");
  }

  auto key = sp[1];
  auto value = sp[2];

  return std::make_unique<Set>(key, value);
}

std::string Set::serialize() const {
  return "set " + this->key + " " + this->value;
}

Result Set::execute(Store &store) const {
  store.set(this->key, this->value);

  return Result{Status::success,
                "the key " + this->key + " has been set to " + this->value};
}

Del::Del(const std::string &_key) : key(_key) {}

const std::string &Del::get_key() const { return this->key; }

std::unique_ptr<Del> Del::deserialize(std::string s) {
  std::vector<std::string> sp = split(s);

  if (sp.size() != 2 || sp[0] != "del") {
    throw UnknownCommand("invalid format for del command. Format: del k");
  }

  auto key = sp[1];

  return std::make_unique<Del>(key);
}

std::string Del::serialize() const { return "del " + this->key; }

Result Del::execute(Store &store) const {
  bool is_exist = store.del(this->key);

  if (!is_exist) {
    return Result{Status::failure_ne,
                  "the key " + this->key + " does not exists"};
  }

  return Result{Status::success, "the key " + this->key + " has been deleted"};
}
