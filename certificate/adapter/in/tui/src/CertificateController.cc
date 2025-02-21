#include "CertificateController.h"
#include "AddCertificateUseCase.h"

#include <iostream>

namespace {
playground::AddCertificateUseCase add_certificate_usecase{};
}

namespace playground {
bool addCertificate(std::string cert) 
{ 
    std::cout << "컨트롤러가 입력을 받았습니다" << std::endl; 

    // input parameter cert could be translated into application.inport's model
    // omitted (for demonstration purpose)
    return add_certificate_usecase.AddCertificate(cert) == 1 ? true : false;
}
} // namespace playground
