#include "LevelDbAccessor.h"

#include <iostream>

namespace playground {
LevelDbAccessor::LevelDbAccessor() {}

bool LevelDbAccessor::update(std::string key, std::string value) {
  std::cout << "DB Accessor가 update를 수행합니다." << std::endl;
  this->memory_db_[key] = value;

  return true;
}

std::string LevelDbAccessor::read(std::string key) {
  // 예외처리 로직은 생략하였습니다
  return this->memory_db_[key];
}
} // namespace playground
