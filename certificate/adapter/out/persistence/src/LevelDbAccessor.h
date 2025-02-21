#pragma once

#include <string>
#include <unordered_map>

namespace playground {
class LevelDbAccessor {
public:
  LevelDbAccessor();

  bool update(std::string key, std::string value);
  std::string read(std::string key);

private:
  // not actually using leveldb
  // fake leveldb accessor, which just uses inmemory db;
  std::unordered_map<std::string, std::string> memory_db_;
};
} // namespace playground
