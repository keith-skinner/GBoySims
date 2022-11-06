#include <gtest/gtest.h>
#include <LR35902/endian.hpp>

TEST(LR35902_endian, constructable)
{
    using namespace LR35902;
    nat16_t myType{5};
    ASSERT_EQ(myType.underlying(), 5);
}

TEST(LR35902_endian, convertable)
{
    using namespace LR35902;
    nat16_t native{5};
    sim16_t converted{static_cast<sim16_t>(native)};
    nat16_t converted_back{static_cast<nat16_t>(converted)};
    ASSERT_EQ(native.underlying(), converted_back.underlying());
}