#include "AddCertificateUseCase.h"
#include "Certificate.h"
#include "UpdateCertificatePort.h"

#include <iostream>

namespace {
playground::UpdateCertificatePort port{};
}

namespace playground {
int AddCertificateUseCase::AddCertificate(std::string cert) {
  // std::string cert is a inport's model (translated from controller's dto)
  // inport's model should be translated to domain model

  std::cout << "inport 구현체인 service가 호출되었습니다" << std::endl;

  Certificate certificate{cert};
  port.update(certificate);

  std::cout << "모든 작업이 완료되었습니다. inport로 응답을 내보냅니다."
            << std::endl;

  return certificate.isValid();
}
} // namespace playground
