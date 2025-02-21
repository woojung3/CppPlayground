#pragma once

#include <string>

namespace playground {
// CertificateData의 scope를 제한하는 편이 좋습니다(외부 모듈에서 접근할 수
// 없도록) Certificate에 inner class로 넣는 등으로 처리할 수 있습니다
struct CertificateData {
  int version{};
  std::string hashed_id{};
  int aux{};
};

class Certificate {
public:
  Certificate(std::string cert);

  std::string getHashedId();
  bool isValid();

private:
  CertificateData data_;
};

// is_standard_layout 구조체인지, compile time에 검사합니다
// 이 검사를 통과한 구조체는 reinterpret_cast등으로 byte로 직접 전환하더라도
// 항상 동일한 개형을 가집니다(표준 구현체임)
static_assert(std::is_standard_layout<CertificateData>::value,
              "직렬화 대상은 standard layout이어야 함");
} // namespace playground
