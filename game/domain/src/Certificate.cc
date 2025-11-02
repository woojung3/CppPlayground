#include "Certificate.h"

#include <iostream>

namespace playground {
Certificate::Certificate(std::string cert) {
  std::cout << "Entity가 생성되었습니다. 영속화되지 않았습니다." << std::endl;

  // 생성자 로직. 페이크로 작성하였습니다
  this->data_.version = 3;
  this->data_.hashed_id =
      "FFFF"; // 유니크한 값이어야 합니다. 페이크로 작성하였습니다
  this->data_.aux = 99;
}

std::string Certificate::getHashedId() { return this->data_.hashed_id; }

bool Certificate::isValid() {
  if (this->data_.version == 3) {
    return true;
  }

  return false;
}
} // namespace playground
