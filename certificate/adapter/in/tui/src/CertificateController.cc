#include "CertificateController.h"
#include "AddCertificateUseCase.h"

#include <iostream>

namespace {
// 유즈케이스를 생성합니다
// 컴파일타임에 빌드도구가 정의와 선언의 결합을 수행하므로 아래와 같이도 코드를
// 작성할 수 있습니다 런타임에 다형성을 적용해야 할 경우, configuration을
// 수행하는 위치(main, test 등)에서 usecase를 주입할 수 있도록 설계를 변경해야
// 합니다
playground::AddCertificateUseCase add_certificate_usecase{};
} // namespace

namespace playground {
bool addCertificate(std::string cert) {
  std::cout << "컨트롤러가 입력을 받았습니다" << std::endl;

  // 입력 파라미터인 문자열 cert는 일종의 DTO입니다
  // DTO는 usecase에 전달되기 전에 usecase model로 변환되어야 합니다
  // (여기에서는 생략합니다)
  return add_certificate_usecase.AddCertificate(cert) == 1 ? true : false;
}
} // namespace playground
