#include <gtest/gtest.h>
#include <LR35902/utility/bits.hpp>
#include <LR35902/arguments.hpp>

GTEST_TEST(Arguments, getOffset)
{
    using namespace LR35902;
    // Zero page access
    EXPECT_EQ(Access::getOffset(Args::zC{}.access), 0xFF00);
    EXPECT_EQ(Access::getOffset(Args::zA8{0x12}.access), 0xFF00);
    // Referenced Args
    EXPECT_EQ(Access::getOffset(Args::rBC{}.access), 0x0000);
    EXPECT_EQ(Access::getOffset(Args::rDE{}.access), 0x0000);
    EXPECT_EQ(Access::getOffset(Args::rHL{}.access), 0x0000);
    EXPECT_EQ(Access::getOffset(Args::rPC{}.access), 0x0000);
    EXPECT_EQ(Access::getOffset(Args::rSP{}.access), 0x0000);
    EXPECT_EQ(Access::getOffset(Args::rA16{0x1234}.access), 0x0000);
    // Won't compile
    // EXPECT_EQ(Access::getOffset(Args::AF{}.access), 0x0000);
    // EXPECT_EQ(Access::getOffset(Args::F{}.access), 0x0000);
}

GTEST_TEST(Arguments, is_std_arg_v)
{
    using namespace LR35902;
    EXPECT_TRUE(Args::is_std_arg_v<Args::A>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::F>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::B>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::C>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::D>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::E>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::H>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::L>);
    EXPECT_TRUE(Args::is_std_arg_v<Args::rHL>);
    EXPECT_FALSE(Args::is_std_arg_v<int>);
}

GTEST_TEST(Arguments, is_reg16_v)
{
    using namespace LR35902;
    EXPECT_TRUE(Args::is_reg16_v<Args::AF>);
    EXPECT_TRUE(Args::is_reg16_v<Args::BC>);
    EXPECT_TRUE(Args::is_reg16_v<Args::DE>);
    EXPECT_TRUE(Args::is_reg16_v<Args::HL>);
    EXPECT_TRUE(Args::is_reg16_v<Args::SP>);
    EXPECT_FALSE(Args::is_reg16_v<int>);
}
