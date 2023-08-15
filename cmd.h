#pragma once

#include "store.h"

#include <memory>
#include <string>
#include <vector>

namespace cmd {
class UnknownCommand : public std::invalid_argument {
public:
  UnknownCommand(const std::string &what_arg);
  UnknownCommand(const char *what_arg);
};

// failure can be of many types, e.g. non-existent failure
// not sure how to tackle this
enum class Status { success, failure, failure_ne };

struct Result {
  Status status;
  std::string payload;
};

class Command {
public:
  static std::unique_ptr<Command> deserialize(std::string);
  virtual std::string serialize() const = 0;
  virtual Result execute(Store &) const = 0;
};

class Get : public Command {
private:
  const std::string key;

public:
  Get(const std::string &);
  const std::string &get_key() const;

  static std::unique_ptr<Get> deserialize(std::string);
  std::string serialize() const override;

  Result execute(Store &) const;
};

class Set : public Command {
private:
  const std::string key;
  const std::string value;

public:
  Set(const std::string &, const std::string &);

  const std::string &get_key() const;
  const std::string &get_value() const;

  static std::unique_ptr<Set> deserialize(std::string);
  std::string serialize() const override;

  Result execute(Store &) const;
};

class Del : public Command {
private:
  const std::string key;

public:
  Del(const std::string &);

  const std::string &get_key() const;

  static std::unique_ptr<Del> deserialize(std::string);
  std::string serialize() const override;

  Result execute(Store &) const;
};
} // namespace cmd
