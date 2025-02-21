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
  // need to add exception handling logic
  // omitted (for demonstration purpose)
  return this->memory_db_[key];
}
} // namespace playground
