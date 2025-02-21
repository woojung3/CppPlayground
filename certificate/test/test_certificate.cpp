#include "CertificateController.h"
#include <gtest/gtest.h>

namespace playground {
TEST(certificate, add) { 
    auto result = addCertificate("80030000000000"); 

    EXPECT_TRUE(result);
}
} // namespace playground
