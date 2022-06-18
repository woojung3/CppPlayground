#include <gtest/gtest.h>

namespace playground {
TEST(str, stricmp)
{
    std::string s1 = "abc00";
    std::string s2 = "ABC00";

    ASSERT_NE(s1, s2);
}
}