#include "LevelDbAccessor.h"
#include "UpdateCertificatePort.h"

#include <iostream>

namespace {
playground::LevelDbAccessor accessor{};
}

namespace playground {
bool UpdateCertificatePort::update(Certificate certificate) {
  std::cout
      << "DB port가 호출되었습니다. Entity영속화를 위해 accessor를 호출합니다."
      << std::endl;

  // leveldb의 경우, 자체 codec이 들어있기 때문에 architecture별로 다르게 저장될
  // 염려는 없음 따라서 is_standard_layout으로 저장 대상 구조체의 직렬화 적합성
  // 여부만 파악한 뒤, 단순 reinterpret_cast로 직렬화/역직렬화를 수행해도 됨
  // 여기에서는 가벼운 데모를 위해 string을 이용함
  // is_standard_layout 컴파일타임 검사코드는 Certificate.h를 참조하시오
  return accessor.update(this->kCertKeyPrefix + certificate.getHashedId(),
                         "serialized data");
}

Certificate UpdateCertificatePort::read(std::string key) {
  // string으로부터 Certificate 복원
  return accessor.read(this->kCertKeyPrefix + key);
}
} // namespace playground
