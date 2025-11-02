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
  // leveldb 연동은 생략하였습니다
  // 인메모리 DB로 대체하였습니다
  std::unordered_map<std::string, std::string> memory_db_;
};
} // namespace playground
