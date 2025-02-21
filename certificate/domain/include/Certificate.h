#pragma once

#include <string>

namespace playground {
// scope를 제한하는 편이 좋음
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

static_assert(std::is_standard_layout<CertificateData>::value, "직렬화 대상은 standard layout이어야 함");
} // namespace playground
