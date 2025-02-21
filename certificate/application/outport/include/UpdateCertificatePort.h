#pragma once

#include "Certificate.h"
#include <string>

namespace playground {
class UpdateCertificatePort {
public:
  bool update(Certificate certificate);
  Certificate read(std::string key);

private:
  // prefix 관리는 분리하지 않았습니다(간단한 데모를 위해)
  std::string kCertKeyPrefix = "cert:";
};
} // namespace playground
