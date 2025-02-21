#pragma once

#include <string>
#include "Certificate.h"

namespace playground {
class UpdateCertificatePort {
    public:
        bool update(Certificate certificate);
        Certificate read(std::string key);

    private:
        std::string kCertKeyPrefix = "cert:";
};
}
